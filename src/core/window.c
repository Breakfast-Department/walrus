#include "walrus/core/window.h"
#include "walrus/backend/backend.h"
#include "walrus/renderer/renderer.h"
#include "walrus/ui/widget.h"
#include <stdlib.h>
#include <stdio.h>

struct WrWindow {
  char* title;
  int width, height;

  int should_close;

  WrRenderSurface *render_surface;
  void* backend_data;
};

WrWindow* wr_create_window(char* title, int width, int height)
{
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

  return window;
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

void wr_window_draw_widget(WrWindow* window, struct WrWidget* widget)
{
  if (!window || !widget)
    return;

  if (widget->render)
  {
    widget->render(widget, window->render_surface);
  }
}
