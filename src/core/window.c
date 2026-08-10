#include "walrus/core/window.h"
#include "walrus/backend/backend.h"
#include <stdlib.h>
#include <stdio.h>

struct WrWindow {
  char* title;
  int width, height;

  int should_close;

  void* backend_data;
};

WrWindow* wr_create_window(char* title, int width, int height)
{
  WrWindow* window = calloc(1, sizeof(WrWindow));
  window->title = title;
  window->width = width;
  window->height = height;
  window->should_close = 0;

  WrBackend* backend = wr_get_backend();
  if (!backend)
  {
    fprintf(stderr, "Backend is not initialized\n");
    return NULL;
  }
  window->backend_data = backend->create_window(title);
  if (!window->backend_data)
  {
    fprintf(stderr, "Failed create backend data\n");
    return NULL;
  }
  return window;
}

void wr_window_destroy(WrWindow* window)
{
  if (!window)
    return;

  WrBackend* backend = wr_get_backend();
  if (!backend)
  {
    fprintf(stderr, "Backend is not initialized\n");
    return;
  }

  backend->destroy_window(window->backend_data);
  free(window);
}
