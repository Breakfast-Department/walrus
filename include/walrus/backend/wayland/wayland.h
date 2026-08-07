#ifndef WALRUS_WAYLAND_H
#define WALRUS_WAYLAND_H

#include "../backend.h"
#include "xdg-shell-client-protocol.h"

extern wr_backend_t wl_backend;

typedef struct wl_window_data {
    struct wl_surface *surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
} wl_window_data_t;

#endif
