#include "walrus/core/window.h"
#include "walrus/backend/backend.h"
#include "walrus/backend/wayland/wayland.h"
#include "walrus/renderer/renderer.h"
#include "walrus/renderer/batch.h"
#include "walrus/ui/widget.h"
#include "walrus/walrus.h"
#include <stdlib.h>
#include <stdio.h>

struct WrWindow {
  char* title;
  int width, height;

  int should_close;

  WrRenderSurface *render_surface;
  void* backend_data;
  WrElement** children;
  unsigned int child_count;
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

static void draw_decorations(WrRenderSurface* surface, int width)
{
  WrRenderer* renderer = wr_get_renderer();
  if (!renderer || !renderer->draw_batch) return;

  WrBatch* batch = wr_batch_create();
  if (!batch) return;

  WrColor titlebar_color = {0.15f, 0.15f, 0.18f, 1.0f};
  wr_batch_rect(batch, 0, 0, (float)width, (float)WR_TITLEBAR_HEIGHT, titlebar_color);

  float close_x = (float)(width - WR_CLOSE_BTN_SIZE - 4);
  float close_y = (float)((WR_TITLEBAR_HEIGHT - WR_CLOSE_BTN_SIZE) / 2);
  WrColor close_color = {0.8f, 0.2f, 0.2f, 1.0f};
  wr_batch_rect(batch, close_x, close_y, (float)WR_CLOSE_BTN_SIZE, (float)WR_CLOSE_BTN_SIZE, close_color);

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

    wr_clear(0.2f, 0.2f, 0.2f, 1.0f);

    draw_decorations(window->render_surface, window->width);

    for (unsigned int j = 0; j < window->child_count; ++j)
    {
      WrElement* el = window->children[j];
      if (!el) continue;
      if (el->type == WR_ELEMENT_TYPE_WIDGET)
      {
        WrWidget* w = (WrWidget*)el->data;
        if (w && w->render)
          w->render(w, window->render_surface);
      }
    }

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
