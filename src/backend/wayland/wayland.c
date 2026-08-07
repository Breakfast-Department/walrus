#include "walrus/backend/wayland/wayland.h"

#include <wayland-client.h>
#include <stdio.h>
#include <string.h>

static struct wl_display *display = NULL;

static int wl_init(void)
{
  display = wl_display_connect(NULL);

  if (!display)
  {
    fprintf(stderr, "Failed to connect to Wayland compositor\n");
    return -1;
  }

  printf("Connected to Wayland\n");

  return 0;
}


static void wl_shutdown(void)
{
  if (display)
  {
    wl_display_disconnect(display);
    display = NULL;
  }

  printf("Disconnected from Wayland\n");
}


static void wl_create_window(wr_window_t *window)
{
  if (!display)
  {
    fprintf(stderr, "Wayland is not initialized\n");
    return;
  }

  /*
    Nanti di sini:
    - wl_registry
    - wl_compositor
    - wl_surface
    - xdg_surface
    - xdg_toplevel
  */

  printf("Create Wayland window\n");
}

static void wl_destroy_window(wr_window_t *window)
{

}

wr_backend_t wl_backend = {
  .type = WR_BACKEND_WAYLAND,
  .init = wl_init,
  .shutdown = wl_shutdown,
  .create_window = wl_create_window,
  .destroy_window = wl_destroy_window,
};
