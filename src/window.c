#include "walrus/walrus.h"
#include "walrus/backend/backend.h"
#include "walrus/window.h"

#include <stdlib.h>
#include <string.h>

wr_window_t *wr_create_window(
  const char *title,
  int width,
  int height
)
{
  wr_backend_t *backend = wr_get_backend();

  if (!backend)
  {
    backend = wr_backend_get();
  }

  if (!backend)
  {
    return NULL;
  }

  wr_window_t *window = malloc(sizeof(wr_window_t));

  if (!window)
  {
    return NULL;
  }

  window->title = malloc(strlen(title) + 1);

  if (!window->title)
  {
    free(window);
    return NULL;
  }

  strcpy(window->title, title);

  window->width = width;
  window->height = height;
  window->should_close = 0;
  window->backend_data = NULL;


    /*
        Backend yang menangani:
        - wl_surface / xdg_toplevel
        - X11 Window
    */

  backend->create_window(window);


  return window;
}

void wr_window_destroy(
  wr_window_t *window
)
{
  wr_backend_t *backend = wr_get_backend();

  if (!window)
  {
    return;
  }


  if (backend && backend->destroy_window)
  {
    backend->destroy_window(window);
  }


  free(window->title);
  free(window);
}

int wr_should_close(wr_window_t *window) {
  return window->should_close;
}

void wr_poll_events() {
  wr_backend_t *backend = wr_get_backend();

  if (backend && backend->poll_events)
    backend->poll_events();
}
