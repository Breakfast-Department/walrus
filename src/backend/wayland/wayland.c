#include "walrus/backend/wayland/wayland.h"
#include "walrus/backend/backend.h"
#include "walrus/backend/wayland/xdg-shell-client-protocol.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <linux/input-event-codes.h>

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

static void xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height, struct wl_array *states);
static void xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel);

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
  .configure = xdg_toplevel_configure,
  .close = xdg_toplevel_close,
  .configure_bounds = NULL,
  .wm_capabilities = NULL,
};

static const struct wl_registry_listener registry_listener = {
  .global = registry_add,
  .global_remove = registry_remove,
};

static WrWaylandWindowData** g_wayland_windows = NULL;
static unsigned int g_wayland_window_count = 0;

static struct wl_seat *g_seat = NULL;
static struct wl_pointer *g_pointer = NULL;
static int g_pointer_x = 0;
static int g_pointer_y = 0;
static struct wl_surface *g_pointer_surface = NULL;
static uint32_t g_last_button_serial = 0;

static void register_wayland_window(WrWaylandWindowData* w) {
  WrWaylandWindowData **nw = realloc(g_wayland_windows, sizeof(*nw) * (g_wayland_window_count + 1));
  if (!nw) return;
  g_wayland_windows = nw;
  g_wayland_windows[g_wayland_window_count++] = w;
}

static void unregister_wayland_window(WrWaylandWindowData* w) {
  if (!g_wayland_windows) return;
  unsigned int i, j = 0;
  for (i = 0; i < g_wayland_window_count; ++i) {
    if (g_wayland_windows[i] == w) continue;
    g_wayland_windows[j++] = g_wayland_windows[i];
  }
  g_wayland_window_count = j;
  if (j == 0) { free(g_wayland_windows); g_wayland_windows = NULL; }
  else {
    WrWaylandWindowData **shr = realloc(g_wayland_windows, sizeof(*shr) * j);
    if (shr) g_wayland_windows = shr;
  }
}

/* pointer listener callbacks */
static void pointer_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                         struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
  (void)data; (void)wl_pointer; (void)serial;
  g_pointer_surface = surface;
  g_pointer_x = wl_fixed_to_int(surface_x);
  g_pointer_y = wl_fixed_to_int(surface_y);
}

static void pointer_leave(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                         struct wl_surface *surface)
{
  (void)data; (void)wl_pointer; (void)serial; (void)surface;
  g_pointer_surface = NULL;
}

static void pointer_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time,
                           wl_fixed_t surface_x, wl_fixed_t surface_y)
{
  (void)data; (void)wl_pointer; (void)time;
  g_pointer_x = wl_fixed_to_int(surface_x);
  g_pointer_y = wl_fixed_to_int(surface_y);
}

static void pointer_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                           uint32_t time, uint32_t button, uint32_t state)
{
  (void)data; (void)wl_pointer; (void)time;
  g_last_button_serial = serial;

  for (unsigned int i = 0; i < g_wayland_window_count; ++i) {
    WrWaylandWindowData* w = g_wayland_windows[i];
    if (!w || !w->app_window) continue;
    if (g_pointer_surface == w->surface) {
      int cur_w = 0, cur_h = 0;
      wr_window_get_size(w->app_window, &cur_w, &cur_h);

      if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
        if (g_pointer_y < WR_TITLEBAR_HEIGHT) {
          int close_x = cur_w - WR_CLOSE_BTN_SIZE - 4;
          if (g_pointer_x >= close_x && g_pointer_x < cur_w - 4) {
            wr_window_set_should_close(w->app_window, 1);
          } else if (w->xdg_toplevel && g_seat) {
            xdg_toplevel_move(w->xdg_toplevel, g_seat, serial);
          }
        }
      }
      break;
    }
  }
}

static void pointer_axis(void *data, struct wl_pointer *wl_pointer, uint32_t time,
                         uint32_t axis, wl_fixed_t value)
{
  (void)data; (void)wl_pointer; (void)time; (void)axis; (void)value;
}

static const struct wl_pointer_listener pointer_listener = {
  .enter = pointer_enter,
  .leave = pointer_leave,
  .motion = pointer_motion,
  .button = pointer_button,
  .axis = pointer_axis,
};

/* seat listener */
static void seat_capabilities(void *data, struct wl_seat *seat, uint32_t caps)
{
  (void)data;
  if (caps & WL_SEAT_CAPABILITY_POINTER) {
    if (!g_pointer && seat) {
      g_pointer = wl_seat_get_pointer(seat);
      wl_pointer_add_listener(g_pointer, &pointer_listener, NULL);
    }
  } else {
    if (g_pointer) {
      wl_pointer_destroy(g_pointer);
      g_pointer = NULL;
    }
  }
}

static void seat_name(void *data, struct wl_seat *seat, const char *name)
{
  (void)data; (void)seat; (void)name;
}

static const struct wl_seat_listener seat_listener = {
  .capabilities = seat_capabilities,
  .name = seat_name,
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
  else if (strcmp(interface, wl_seat_interface.name) == 0)
  {
    g_seat = wl_registry_bind(
      registry,
      name,
      &wl_seat_interface,
      1
    );
    if (g_seat)
      wl_seat_add_listener(g_seat, &seat_listener, NULL);
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
    fprintf(stderr, "Wayland backend: failed to connect to compositor\n");
    free(wr_wayland);
    wr_wayland = NULL;
    return -1;
  }

  wr_wayland->registry =
    wl_display_get_registry(wr_wayland->display);

  if (!wr_wayland->registry)
  {
    fprintf(stderr, "Wayland backend: failed to get registry\n");
    wl_display_disconnect(wr_wayland->display);
    free(wr_wayland);
    wr_wayland = NULL;
    return -1;
  }

  wl_registry_add_listener(
    wr_wayland->registry,
    &registry_listener,
    NULL
  );

  if (wl_display_roundtrip(wr_wayland->display) == -1)
  {
    fprintf(stderr, "Wayland backend: display roundtrip failed\n");
    wl_display_disconnect(wr_wayland->display);
    free(wr_wayland);
    wr_wayland = NULL;
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

static void* create_window(void* app_window, char* title, int width, int height) {
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
    fprintf(stderr, "Failed to create xdg_toplevel\n");

    xdg_surface_destroy(data->xdg_surface);
    wl_surface_destroy(data->surface);
    free(data);

    return NULL;
  }

  data->app_window = (WrWindow*)app_window;

  register_wayland_window(data);

  xdg_toplevel_add_listener(data->xdg_toplevel, &xdg_toplevel_listener, data);

  data->egl_window = wl_egl_window_create(data->surface, width, height);
  if (!data->egl_window)
  {
    fprintf(stderr, "Failed to create wl_egl_window\n");

    xdg_toplevel_destroy(data->xdg_toplevel);
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
  wl_display_roundtrip(wr_wayland->display);
  return data;
}

static void destroy_window(void* ptr_data) {
  WrWaylandWindowData* data = ptr_data;

  unregister_wayland_window(data);

  if (data->egl_window)
    wl_egl_window_destroy(data->egl_window);

  if (data->xdg_toplevel)
    xdg_toplevel_destroy(data->xdg_toplevel);

  if (data->xdg_surface)
    xdg_surface_destroy(data->xdg_surface);

  if (data->surface)
    wl_surface_destroy(data->surface);

  free(data);
}

static void* get_native_window(void* ptr_data)
{
  WrWaylandWindowData* data = ptr_data;
  return data ? data->egl_window : NULL;
}

static void wr_wayland_resize_window(void* ptr_data, int width, int height)
{
  WrWaylandWindowData* data = ptr_data;
  if (!data || !data->egl_window) return;
  wl_egl_window_resize(data->egl_window, width, height, 0, 0);
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

static void xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
  (void)xdg_toplevel;
  WrWaylandWindowData *window = data;
  if (!window)
    return;

  if (window->app_window)
    wr_window_set_should_close(window->app_window, 1);
}

static void xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height, struct wl_array *states)
{
  (void)xdg_toplevel;
  (void)states;
  WrWaylandWindowData *window = data;
  if (!window) return;

  /* If compositor provided a suggested size, forward as an input event so
     the application can update internal layout and renderer surfaces. */
  if (width > 0 && height > 0) {
    WrInputEvent ev;
    ev.type = WR_INPUT_EVENT_WINDOW_RESIZE;
    ev.data.resize.width = width;
    ev.data.resize.height = height;

    if (window->app_window)
      wr_window_handle_input(window->app_window, &ev);

    /* Resize the EGL window to match suggested size */
    if (window->egl_window)
      wl_egl_window_resize(window->egl_window, width, height, 0, 0);
  }
}

static void* get_native_display(void)
{
  return wr_wayland->display;
}

WrBackend wr_wayland_backend = {
  .init = init,
  .shutdown = shutdown,
  .poll_events = poll_events,
  .create_window = create_window,
  .destroy_window = destroy_window,
  .get_native_display = get_native_display,
  .get_native_window = get_native_window,
  .resize_window = wr_wayland_resize_window
};
