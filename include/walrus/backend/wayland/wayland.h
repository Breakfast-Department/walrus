#ifndef WR_WAYLAND_H
#define WR_WAYLAND_H

#include <walrus/backend/backend.h>
#include <wayland-client-protocol.h>

extern WrBackend wr_wayland_backend;

typedef struct WrWayland {
  struct wl_display* display;
  struct wl_registry* registry;

  struct wl_compositor* compositor;
  struct xdg_wm_base* wm_base;
} WrWayland;

typedef struct WrWaylandWindowData {
  struct wl_surface *surface;
  struct xdg_surface *xdg_surface;
  struct xdg_toplevel *xdg_toplevel;
} WrWaylandWindowData;

#endif
