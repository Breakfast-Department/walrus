#include <walrus/backend/x11/x11.h>
#include <walrus/core/window.h>
#include <walrus/core/input.h>
#include <stddef.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct WrX11Window {
  Window window;
  WrWindow *app_window;
  int resizing;
  int resize_start_x;
  int resize_start_y;
  int resize_start_w;
  int resize_start_h;
} WrX11Window;

static Display *wr_display = NULL;

static WrX11Window** g_x11_windows = NULL;
static unsigned int g_x11_window_count = 0;

static void register_x11_window(WrX11Window* w) {
  WrX11Window** nw = realloc(g_x11_windows, sizeof(WrX11Window*) * (g_x11_window_count + 1));
  if (!nw) return;
  g_x11_windows = nw;
  g_x11_windows[g_x11_window_count++] = w;
}

static void unregister_x11_window(WrX11Window* w) {
  if (!w) return;
  unsigned int i, dst = 0;
  for (i = 0; i < g_x11_window_count; ++i) {
    if (g_x11_windows[i] == w) continue;
    g_x11_windows[dst++] = g_x11_windows[i];
  }
  g_x11_window_count = dst;
  if (dst == 0) { free(g_x11_windows); g_x11_windows = NULL; }
  else {
    WrX11Window** sh = realloc(g_x11_windows, sizeof(WrX11Window*) * dst);
    if (sh) g_x11_windows = sh;
  }
}

static int wr_x11_init(void)
{
  wr_display = XOpenDisplay(NULL);
  if (!wr_display)
  {
    fprintf(stderr, "X11: failed to open display\n");
    return -1;
  }

  return 0;
}

static void wr_x11_shutdown(void)
{
  if (wr_display)
  {
    XCloseDisplay(wr_display);
    wr_display = NULL;
  }
}

static void wr_x11_poll_events(void)
{
  if (!wr_display)
    return;

  while (XPending(wr_display))
  {
    XEvent ev;
    XNextEvent(wr_display, &ev);
    if (ev.type == ConfigureNotify) {
      XConfigureEvent *cfg = (XConfigureEvent*)&ev;
      unsigned int i;
      for (i = 0; i < g_x11_window_count; ++i) {
        WrX11Window* w = g_x11_windows[i];
        if (!w) continue;
        if (w->window == cfg->window) {
          if (w->app_window) {
            WrInputEvent iev;
            iev.type = WR_INPUT_EVENT_WINDOW_RESIZE;
            iev.data.resize.width = cfg->width;
            iev.data.resize.height = cfg->height;
            wr_window_handle_input(w->app_window, &iev);
          }
          break;
        }
      }
    } else if (ev.type == ButtonPress) {
      XButtonEvent *bev = (XButtonEvent*)&ev.xbutton;
      unsigned int i;
      for (i = 0; i < g_x11_window_count; ++i) {
        WrX11Window* w = g_x11_windows[i];
        if (!w) continue;
        if (w->window == bev->window) {
          if (!w->app_window) break;

          if (bev->button == Button3) {
            XWindowAttributes attrs;
            if (!XGetWindowAttributes(wr_display, w->window, &attrs)) break;
            w->resizing = 1;
            w->resize_start_x = bev->x_root;
            w->resize_start_y = bev->y_root;
            w->resize_start_w = attrs.width;
            w->resize_start_h = attrs.height;
          }
          break;
        }
      }
    } else if (ev.type == MotionNotify) {
      XMotionEvent *mev = (XMotionEvent*)&ev.xmotion;
      unsigned int i;
      for (i = 0; i < g_x11_window_count; ++i) {
        WrX11Window* w = g_x11_windows[i];
        if (!w) continue;
        if (w->window == mev->window) {
          if (w->resizing && w->app_window) {
            int dx = mev->x_root - w->resize_start_x;
            int dy = mev->y_root - w->resize_start_y;
            int new_w = w->resize_start_w + dx;
            int new_h = w->resize_start_h + dy;
            if (new_w < 100) new_w = 100;
            if (new_h < 60) new_h = 60;
            wr_window_set_size(w->app_window, new_w, new_h);
          }
          break;
        }
      }
    } else if (ev.type == ButtonRelease) {
      XButtonEvent *brev = (XButtonEvent*)&ev.xbutton;
      unsigned int i;
      for (i = 0; i < g_x11_window_count; ++i) {
        WrX11Window* w = g_x11_windows[i];
        if (!w) continue;
        if (w->window == brev->window) {
          if (brev->button == Button3) {
            w->resizing = 0;
          }
          break;
        }
      }
    } else if (ev.type == KeyPress) {
      XKeyEvent *kev = (XKeyEvent*)&ev.xkey;
      KeySym ks = XLookupKeysym(kev, 0);
      unsigned int i;
      for (i = 0; i < g_x11_window_count; ++i) {
        WrX11Window* w = g_x11_windows[i];
        if (!w) continue;
        if (w->window == kev->window) {
          if (!w->app_window) break;

          XWindowAttributes attrs;
          if (!XGetWindowAttributes(wr_display, w->window, &attrs)) break;
          int new_w = attrs.width;
          int new_h = attrs.height;

          if (ks == (KeySym)'+') {
            new_w += 50; new_h += 30;
            wr_window_set_size(w->app_window, new_w, new_h);
          } else if (ks == (KeySym)'-') {
            new_w = (new_w > 100) ? new_w - 50 : new_w;
            new_h = (new_h > 60) ? new_h - 30 : new_h;
            wr_window_set_size(w->app_window, new_w, new_h);
          }
          break;
        }
      }
    }
  }
}

static void* wr_x11_create_window(void* native_display, char* title, int width, int height)
{
  void* app_window = native_display;
  if (!wr_display)
    return NULL;

  WrX11Window *data = calloc(1, sizeof(*data));
  if (!data)
    return NULL;

  int screen = DefaultScreen(wr_display);
  Window root = RootWindow(wr_display, screen);

  data->window = XCreateSimpleWindow(
    wr_display,
    root,
    0, 0,
    (unsigned)width, (unsigned)height,
    0,
    BlackPixel(wr_display, screen),
    WhitePixel(wr_display, screen)
  );

  data->app_window = (WrWindow*)app_window;

  XSelectInput(wr_display, data->window, StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

  XStoreName(wr_display, data->window, title);
  XMapWindow(wr_display, data->window);
  XFlush(wr_display);

  register_x11_window(data);

  return data;
}

static void wr_x11_destroy_window(void* ptr)
{
  WrX11Window *data = ptr;
  if (!data)
    return;

  if (wr_display && data->window)
    XDestroyWindow(wr_display, data->window);

  unregister_x11_window(data);
  free(data);
}

static void* wr_x11_get_native_display(void)
{
  return wr_display;
}

static void* wr_x11_get_native_window(void* ptr)
{
  WrX11Window *data = ptr;
  if (!data)
    return NULL;

  return (void*)(uintptr_t)data->window;
}

static void wr_x11_resize_window(void* ptr, int width, int height)
{
  WrX11Window *data = ptr;
  if (!data || !wr_display) return;
  XResizeWindow(wr_display, data->window, (unsigned)width, (unsigned)height);
  XFlush(wr_display);
}

WrBackend wr_x11_backend = {
  .init = wr_x11_init,
  .shutdown = wr_x11_shutdown,
  .poll_events = wr_x11_poll_events,
  .create_window = wr_x11_create_window,
  .destroy_window = wr_x11_destroy_window,
  .get_native_display = wr_x11_get_native_display,
  .get_native_window = wr_x11_get_native_window,
  .resize_window = wr_x11_resize_window,
};
