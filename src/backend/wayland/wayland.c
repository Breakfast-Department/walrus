#include "walrus/backend/wayland/wayland.h"

#include <wayland-client.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static struct wl_display *display = NULL;

static struct wl_registry *registry = NULL;
static struct wl_compositor *compositor = NULL;
static struct xdg_wm_base *wm_base = NULL;

static void xdg_surface_configure(
    void *data,
    struct xdg_surface *surface,
    uint32_t serial
);

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void registry_add(
    void *data,
    struct wl_registry *registry,
    uint32_t name,
    const char *interface,
    uint32_t version)
{
    if (strcmp(interface, wl_compositor_interface.name) == 0)
    {
        compositor = wl_registry_bind(
            registry,
            name,
            &wl_compositor_interface,
            4
        );
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        wm_base = wl_registry_bind(
            registry,
            name,
            &xdg_wm_base_interface,
            1
        );
    }
}

static void registry_remove(
    void *data,
    struct wl_registry *registry,
    uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_add,
    .global_remove = registry_remove,
};

static int wl_init(void)
{
  display = wl_display_connect(NULL);

  if (!display)
  {
    fprintf(stderr, "Failed to connect to Wayland compositor\n");
    return -1;
  }

  display = wl_display_connect(NULL);

  registry = wl_display_get_registry(display);

  wl_registry_add_listener(
      registry,
      &registry_listener,
      NULL
  );

  wl_display_roundtrip(display);

  return 0;
}

static void wl_create_window(wr_window_t *window)
{
  if (!display)
  {
    fprintf(stderr, "Wayland is not initialized\n");
    return;
  }

  if (!compositor || !wm_base)
  {
      fprintf(stderr, "Wayland is not ready\n");
      return;
  }

  wl_window_data_t *data = calloc(1, sizeof(wl_window_data_t));

  data->surface = wl_compositor_create_surface(compositor);

  data->xdg_surface =
      xdg_wm_base_get_xdg_surface(
          wm_base,
          data->surface
      );
    
  xdg_surface_add_listener(
      data->xdg_surface,
      &xdg_surface_listener,
      data
  );

  data->xdg_toplevel =
      xdg_surface_get_toplevel(
          data->xdg_surface
      );

  xdg_toplevel_set_title(
      data->xdg_toplevel,
      window->title
  );

  wl_surface_commit(data->surface);

  window->backend_data = data;
}

static void xdg_surface_configure(
    void *data,
    struct xdg_surface *surface,
    uint32_t serial)
{
    xdg_surface_ack_configure(surface, serial);

    wl_window_data_t *window = data;
    wl_surface_commit(window->surface);
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

static void wl_destroy_window(wr_window_t *window)
{
    if (!window || !window->backend_data)
        return;

    wl_window_data_t *data = window->backend_data;

    if (data->xdg_toplevel)
        xdg_toplevel_destroy(data->xdg_toplevel);

    if (data->xdg_surface)
        xdg_surface_destroy(data->xdg_surface);

    if (data->surface)
        wl_surface_destroy(data->surface);

    free(data);
    window->backend_data = NULL;
}

static void wl_poll_events(void)
{
  if (!display)
    return;

  wl_display_dispatch_pending(display);
  wl_display_flush(display);
}

wr_backend_t wl_backend = {
  .type = WR_BACKEND_WAYLAND,
  .init = wl_init,
  .poll_events = wl_poll_events,
  .shutdown = wl_shutdown,
  .create_window = wl_create_window,
  .destroy_window = wl_destroy_window,
};
