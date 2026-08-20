#include "walrus/core/window.h"
#include "walrus/backend/backend.h"
#include "walrus/backend/wayland/wayland.h"
#include "walrus/renderer/renderer.h"
#include "walrus/renderer/batch.h"
#include "walrus/text/font.h"
#include "walrus/text/text.h"
#include "walrus/ui/widget.h"
#include "walrus/walrus.h"
#include <glad/glad.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct WrWindow {
  char* title;
  int width, height;

  int should_close;

  WrRenderSurface *render_surface;
  void* backend_data;

  WrElement** children;
  unsigned int child_count;

  WrElement** decoration_children;
  unsigned int decoration_child_count;
};

static WrWindow** g_windows = NULL;
static unsigned int g_window_count = 0;

static void register_window(WrWindow* window);

WrWindow* wr_create_window(char* title, int width, int height)
{
  if (width == 0 || height == 0)
  {
    fprintf(stderr, "Window size is invalid\n");
    return NULL;
  }

  WrWindow* window = calloc(1, sizeof(WrWindow));
  if (!window)
    return NULL;

  window->title = title;
  window->width = width;
  window->height = height;
  window->should_close = 0;
  window->render_surface = NULL;

  WrBackend* backend = wr_get_backend();
  if (!backend)
  {
    fprintf(stderr, "Backend is not initialized\n");
    free(window);
    return NULL;
  }

  window->backend_data = backend->create_window(window, title, width, height);
  if (!window->backend_data)
  {
    fprintf(stderr, "Failed create backend data\n");
    free(window);
    return NULL;
  }

  WrRenderer* renderer = wr_get_renderer();
  if (renderer && renderer->create_surface)
  {
    void *native_window = backend->get_native_window(window->backend_data);
    window->render_surface = renderer->create_surface(native_window);
    if (!window->render_surface)
    {
      backend->destroy_window(window->backend_data);
      free(window);
      return NULL;
    }
  }

  register_window(window);

  return window;
}

static void register_window(WrWindow* window)
{
  WrWindow** nw = realloc(g_windows, sizeof(WrWindow*) * (g_window_count + 1));
  if (!nw) return;
  g_windows = nw;
  g_windows[g_window_count++] = window;
}

static void unregister_window(WrWindow* window)
{
  if (!window) return;
  unsigned int i, dst = 0;
  for (i = 0; i < g_window_count; ++i) {
    if (g_windows[i] == window) continue;
    g_windows[dst++] = g_windows[i];
  }
  g_window_count = dst;
  if (dst == 0) {
    free(g_windows);
    g_windows = NULL;
  } else {
    WrWindow** shrink = realloc(g_windows, sizeof(WrWindow*) * dst);
    if (shrink) g_windows = shrink;
  }
}

void wr_window_destroy(WrWindow* window)
{
  if (!window)
    return;

  WrRenderer* renderer = wr_get_renderer();
  if (renderer && renderer->destroy_surface && window->render_surface)
  {
    renderer->destroy_surface(window->render_surface);
    window->render_surface = NULL;
  }

  WrBackend* backend = wr_get_backend();
  if (!backend)
  {
    fprintf(stderr, "Backend is not initialized\n");
    free(window);
    return;
  }

  backend->destroy_window(window->backend_data);
  unregister_window(window);
  free(window->children);
  free(window->decoration_children);
  free(window);
}

WrRenderSurface* wr_window_get_surface(WrWindow* window)
{
  if (!window)
    return NULL;

  return window->render_surface;
}

int wr_window_should_close(WrWindow* window)
{
  if (!window)
    return 1;
  return window->should_close;
}

void wr_window_set_should_close(WrWindow* window, int value)
{
  if (!window)
    return;
  window->should_close = value;
}

void wr_window_draw_widget(WrWindow* window, WrWidget* widget)
{
  if (!window || !widget)
    return;

  if (widget->render)
  {
    widget->render(widget, window->render_surface);
  }
}

void wr_window_set_size(WrWindow* window, int width, int height)
{
  if (!window) return;
  window->width = width;
  window->height = height;

  WrBackend* backend = wr_get_backend();
  if (backend && backend->resize_window && window->backend_data)
  {
    backend->resize_window(window->backend_data, width, height);
  }

  /* Note: the backend's resize_window already handles wl_egl_window_resize
     for Wayland, so we don't need to recreate the EGL surface here. */

  for (unsigned int i = 0; i < window->child_count; ++i) {
    WrElement* el = window->children[i];
    if (!el) continue;
    if (el->type == WR_ELEMENT_TYPE_WIDGET) {
      WrWidget* w = (WrWidget*)el->data;
      if (!w) continue;
      w->style.layout.width = (float)(window->width / 3);
      w->style.layout.height = (float)(window->height / 4);
    }
  }
}

void wr_window_handle_input(WrWindow* window, WrInputEvent* ev)
{
  if (!window || !ev) return;
  switch (ev->type)
  {
    case WR_INPUT_EVENT_WINDOW_RESIZE:
      wr_window_set_size(window, ev->data.resize.width, ev->data.resize.height);
      break;
    default:
      break;
  }
}

int wr_window_get_size(WrWindow* window, int* width, int* height)
{
  if (!window) return -1;
  if (width) *width = window->width;
  if (height) *height = window->height;
  return 0;
}

static WrFont* g_ui_font = NULL;

static void ensure_ui_font(void)
{
  if (g_ui_font)
    return;

  const char* font_paths[] = {
    "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/noto/NotoSans-Regular.ttf",
    NULL
  };

  for (int i = 0; font_paths[i]; i++) {
    g_ui_font = wr_font_load(font_paths[i], 13.0f);
    if (g_ui_font)
      break;
  }
}

static void draw_decorations(WrRenderSurface* surface, WrWindow* window)
{
  WrRenderer* renderer = wr_get_renderer();
  if (!renderer || !renderer->draw_batch) return;

  WrBatch* batch = wr_batch_create();
  if (!batch) return;

  int width = window->width;
  int height = window->height;
  int maximized = wr_window_is_maximized();

  const float corner_radius = maximized ? 0.0f : 16.0f;
  const float border_width = maximized ? 0.0f : 1.0f;
  const float btn_radius = 6.0f;
  const float btn_spacing = 8.0f;
  const float content_margin = maximized ? 0.0f : 8.0f;
  const float content_radius = maximized ? 0.0f : 8.0f;

  WrColor frame_color = {0.18f, 0.18f, 0.20f, 1.0f};

  if (maximized) {
    wr_batch_rect(batch, 0, 0, (float)width, (float)height, frame_color);
  } else {
    wr_batch_rounded_rect(batch, 0, 0, (float)width, (float)height, corner_radius, frame_color);

    WrColor border_color = {0.30f, 0.30f, 0.32f, 1.0f};
    wr_batch_rounded_rect(batch, 0, 0, (float)width, (float)height, corner_radius, border_color);
    wr_batch_rounded_rect(batch, border_width, border_width,
      (float)width - border_width * 2, (float)height - border_width * 2,
      corner_radius - border_width, frame_color);
  }

  float btn_y = (float)WR_TITLEBAR_HEIGHT / 2.0f;
  float btn_margin = maximized ? 8.0f : 8.0f;
  float btn_x = (float)width - btn_margin - btn_radius;

  WrColor close_color = {0.94f, 0.36f, 0.36f, 1.0f};
  wr_batch_circle(batch, btn_x, btn_y, btn_radius, close_color);

  btn_x -= (btn_radius * 2 + btn_spacing);
  WrColor maximize_color = {0.98f, 0.76f, 0.29f, 1.0f};
  wr_batch_circle(batch, btn_x, btn_y, btn_radius, maximize_color);

  btn_x -= (btn_radius * 2 + btn_spacing);
  WrColor minimize_color = {0.40f, 0.84f, 0.40f, 1.0f};
  wr_batch_circle(batch, btn_x, btn_y, btn_radius, minimize_color);

  float content_x = content_margin;
  float content_y = (float)WR_TITLEBAR_HEIGHT;
  float content_w = (float)width - content_margin * 2;
  float content_h = (float)height - (float)WR_TITLEBAR_HEIGHT - content_margin;

  if (maximized) {
    WrColor content_bg = {0.12f, 0.12f, 0.14f, 1.0f};
    wr_batch_rect(batch, content_x, content_y, content_w, content_h, content_bg);
  } else {
    WrColor content_border = {0.25f, 0.25f, 0.28f, 1.0f};
    wr_batch_rounded_rect(batch, content_x, content_y, content_w, content_h, content_radius, content_border);

    WrColor content_bg = {0.12f, 0.12f, 0.14f, 1.0f};
    wr_batch_rounded_rect(batch, content_x + border_width, content_y + border_width,
      content_w - border_width * 2, content_h - border_width * 2,
      content_radius - border_width, content_bg);
  }

  /* Draw window title */
  ensure_ui_font();
  if (g_ui_font && window->title) {
    WrColor title_color = {0.85f, 0.85f, 0.87f, 1.0f};
    float title_x = (float)width / 2.0f;
    float title_y = (float)WR_TITLEBAR_HEIGHT / 2.0f;
    wr_batch_text_aligned(batch, g_ui_font, window->title, title_x, title_y,
      title_color, WR_TEXT_ALIGN_CENTER, WR_TEXT_BASELINE_MIDDLE);
  }

  renderer->draw_batch(surface, batch);
  wr_batch_destroy(batch);
}

void wr_render(void)
{
  for (unsigned int i = 0; i < g_window_count; ++i)
  {
    WrWindow* window = g_windows[i];
    if (!window || !window->render_surface) continue;

    if (wr_begin_frame(window->render_surface) != 0)
      continue;

    wr_clear(0.1f, 0.1f, 0.12f, 1.0f);

    draw_decorations(window->render_surface, window);

    for (unsigned int j = 0; j < window->decoration_child_count; ++j)
    {
      WrElement* el = window->decoration_children[j];
      if (!el) continue;
      if (el->type == WR_ELEMENT_TYPE_WIDGET)
      {
        WrWidget* w = (WrWidget*)el->data;
        if (w && w->render)
          w->render(w, window->render_surface);
      }
    }

    float content_x, content_y, content_w, content_h;
    wr_window_get_content_bounds(window, &content_x, &content_y, &content_w, &content_h);

    /* Enable scissor test to clip children to content area */
    glEnable(GL_SCISSOR_TEST);
    glScissor(
      (GLint)content_x,
      (GLint)(window->height - content_y - content_h),
      (GLsizei)content_w,
      (GLsizei)content_h
    );

    for (unsigned int j = 0; j < window->child_count; ++j)
    {
      WrElement* el = window->children[j];
      if (!el) continue;
      if (el->type == WR_ELEMENT_TYPE_WIDGET)
      {
        WrWidget* w = (WrWidget*)el->data;
        if (w && w->render) {
          w->style.layout.margin.left = content_x;
          w->style.layout.margin.top = content_y;
          w->render(w, window->render_surface);
        }
      }
    }

    glDisable(GL_SCISSOR_TEST);

    wr_end_frame(window->render_surface);
  }
}

int wr_window_add_child(WrWindow* window, WrElement* element)
{
  if (!window || !element) return -1;
  WrElement** nc = realloc(window->children, sizeof(WrElement*) * (window->child_count + 1));
  if (!nc) return -1;
  window->children = nc;
  window->children[window->child_count++] = element;
  return 0;
}

int wr_window_remove_child(WrWindow* window, WrElement* element)
{
  if (!window || !element) return -1;
  unsigned int i, dst = 0;
  for (i = 0; i < window->child_count; ++i) {
    if (window->children[i] == element) continue;
    window->children[dst++] = window->children[i];
  }
  window->child_count = dst;
  if (dst == 0) { free(window->children); window->children = NULL; }
  else {
    WrElement** shrink = realloc(window->children, sizeof(WrElement*) * dst);
    if (shrink) window->children = shrink;
  }
  return 0;
}

void wr_request_close_all(void)
{
  for (unsigned int i = 0; i < g_window_count; ++i) {
    WrWindow* w = g_windows[i];
    if (w) w->should_close = 1;
  }
}

int wr_window_add_decoration(WrWindow* window, WrElement* element)
{
  if (!window || !element) return -1;
  WrElement** nc = realloc(window->decoration_children,
    sizeof(WrElement*) * (window->decoration_child_count + 1));
  if (!nc) return -1;
  window->decoration_children = nc;
  window->decoration_children[window->decoration_child_count++] = element;
  return 0;
}

int wr_window_remove_decoration(WrWindow* window, WrElement* element)
{
  if (!window || !element) return -1;
  unsigned int i, dst = 0;
  for (i = 0; i < window->decoration_child_count; ++i) {
    if (window->decoration_children[i] == element) continue;
    window->decoration_children[dst++] = window->decoration_children[i];
  }
  window->decoration_child_count = dst;
  if (dst == 0) {
    free(window->decoration_children);
    window->decoration_children = NULL;
  } else {
    WrElement** shrink = realloc(window->decoration_children, sizeof(WrElement*) * dst);
    if (shrink) window->decoration_children = shrink;
  }
  return 0;
}

void wr_window_get_content_bounds(WrWindow* window, float* x, float* y, float* w, float* h)
{
  if (!window) return;

  int maximized = wr_window_is_maximized();
  float content_margin = maximized ? 0.0f : 8.0f;
  float border_width = maximized ? 0.0f : 1.0f;

  float cx = content_margin + border_width;
  float cy = (float)WR_TITLEBAR_HEIGHT + border_width;
  float cw = (float)window->width - (content_margin + border_width) * 2;
  float ch = (float)window->height - (float)WR_TITLEBAR_HEIGHT - content_margin - border_width * 2;

  if (x) *x = cx;
  if (y) *y = cy;
  if (w) *w = cw;
  if (h) *h = ch;
}

void wr_window_cleanup_ui(void)
{
  if (g_ui_font) {
    wr_font_destroy(g_ui_font);
    g_ui_font = NULL;
  }
}
