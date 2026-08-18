#ifndef WR_WAYLAND_H
#define WR_WAYLAND_H

#define WR_TITLEBAR_HEIGHT 32
#define WR_BORDER_WIDTH 4
#define WR_CLOSE_BTN_SIZE 24
#define WR_RESIZE_BORDER 8
#define WR_MIN_WIDTH 150
#define WR_MIN_HEIGHT 80

#include <walrus/backend/backend.h>
#include <walrus/core/window.h>
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
  struct wl_egl_window *egl_window;
  WrWindow *app_window;
  int last_width, last_height;
  uint32_t resize_edge;
} WrWaylandWindowData;

#endif
