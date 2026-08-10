#include "walrus/backend/wayland/wayland.h"
#include "walrus/backend/backend.h"
#include "walrus/backend/wayland/xdg-shell-client-protocol.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

static WrWayland *wr_wayland = NULL;

static void xdg_surface_configure(
  void *data,
  struct xdg_surface *surface,
  uint32_t serial
);

static void registry_add(
  void* data,
  struct wl_registry* registry,
  uint32_t name,
  const char* interface,
  uint32_t version
);
static void registry_remove(
  void* data,
  struct wl_registry* registry,
  uint32_t name
);

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_configure,
};

static const struct wl_registry_listener registry_listener = {
    .global = registry_add,
    .global_remove = registry_remove,
};
static void registry_add(
    void *data,
    struct wl_registry *registry,
    uint32_t name,
    const char *interface,
    uint32_t version
)
{
    (void)data;
    (void)version;

    if (strcmp(interface, wl_compositor_interface.name) == 0)
    {
        wr_wayland->compositor = wl_registry_bind(
            registry,
            name,
            &wl_compositor_interface,
            4
        );
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        wr_wayland->wm_base = wl_registry_bind(
            registry,
            name,
            &xdg_wm_base_interface,
            1
        );
    }
}

static void registry_remove(
  void* data,
  struct wl_registry* registry,
  uint32_t name
) {
  (void)data;
  (void)registry;
  (void)name;
}

static int init(void)
{
  wr_wayland = calloc(1, sizeof(*wr_wayland));

  if (!wr_wayland)
  {
    fprintf(stderr, "Failed to allocate WrWayland\n");
    return -1;
  }

  wr_wayland->display = wl_display_connect(NULL);

  if (!wr_wayland->display)
  {
    fprintf(stderr, "Failed to connect to Wayland compositor\n");
    free(wr_wayland);
    wr_wayland = NULL;
    return -1;
  }

  wr_wayland->registry =
    wl_display_get_registry(wr_wayland->display);

  wl_registry_add_listener(
    wr_wayland->registry,
    &registry_listener,
    NULL
  );

  if (wl_display_roundtrip(wr_wayland->display) == -1)
  {
    fprintf(stderr, "Wayland roundtrip failed\n");
    return -1;
  }

  return 0;
}

static void shutdown(void) {
  if (wr_wayland->display)
  {
    wl_display_disconnect(wr_wayland->display);
    wr_wayland->display = NULL;
  }
}

static void poll_events(void) {
  if (!wr_wayland->display)
    return;

  wl_display_dispatch_pending(wr_wayland->display);
  wl_display_flush(wr_wayland->display);
}

static void* create_window(char* title) {
  if (!wr_wayland || !wr_wayland->display)
  {
    fprintf(stderr, "Wayland is not initialized\n");
    return NULL;
  }

  if (!wr_wayland->compositor || !wr_wayland->wm_base)
  {
    fprintf(stderr, "Wayland is not ready\n");
    return NULL;
  }

  WrWaylandWindowData* data = calloc(1, sizeof(WrWaylandWindowData));

  if (!data)
  {
    fprintf(stderr, "Failed to allocate window data\n");
    return NULL;
  }

  data->surface = wl_compositor_create_surface(wr_wayland->compositor);
  if (!data->surface)
  {
    fprintf(stderr, "Failed to create wl_surface\n");
    free(data);
    return NULL;
  }

  data->xdg_surface = xdg_wm_base_get_xdg_surface(
    wr_wayland->wm_base,
    data->surface
  );

  if (!data->xdg_surface)
  {
    fprintf(stderr, "Failed to create xdg_surface\n");

    wl_surface_destroy(data->surface);
    free(data);

    return NULL;
  }

  xdg_surface_add_listener(
    data->xdg_surface,
    &xdg_surface_listener,
    data
  );

  data->xdg_toplevel = xdg_surface_get_toplevel(
    data->xdg_surface
  );

  if (!data->xdg_toplevel)
  {
    fprintf(stderr, "Failed to create xdg_toplevel");

    xdg_surface_destroy(data->xdg_surface);
    wl_surface_destroy(data->surface);
    free(data);

    return NULL;
  }

  xdg_toplevel_set_title(
    data->xdg_toplevel,
    title
  );

  wl_surface_commit(data->surface);
  return data;
}

static void destroy_window(void* ptr_data) {
  WrWaylandWindowData* data = ptr_data;
  if (data->xdg_toplevel)
    xdg_toplevel_destroy(data->xdg_toplevel);

  if (data->xdg_surface)
    xdg_surface_destroy(data->xdg_surface);

  if (data->surface)
    wl_surface_destroy(data->surface);

  free(data);
}

static void xdg_surface_configure(
  void *data,
  struct xdg_surface *surface,
  uint32_t serial
) {
  xdg_surface_ack_configure(surface, serial);

  WrWaylandWindowData *window = data;
  wl_surface_commit(window->surface);
}

WrBackend wr_wayland_backend = {
  .init = init,
  .shutdown = shutdown,
  .create_window = create_window,
  .destroy_window = destroy_window,
  .poll_events = poll_events
};
