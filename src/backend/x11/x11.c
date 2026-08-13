#include <walrus/backend/x11/x11.h>
#include <stddef.h>
#include <X11/Xlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct WrX11Window {
  Window window;
} WrX11Window;

static Display *wr_display = NULL;

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
    (void)ev;
  }
}

static void* wr_x11_create_window(void* native_display, char* title, int width, int height)
{
  (void)native_display;
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

  XStoreName(wr_display, data->window, title);
  XMapWindow(wr_display, data->window);
  XFlush(wr_display);

  return data;
}

static void wr_x11_destroy_window(void* ptr)
{
  WrX11Window *data = ptr;
  if (!data)
    return;

  if (wr_display && data->window)
    XDestroyWindow(wr_display, data->window);

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

WrBackend wr_x11_backend = {
  .init = wr_x11_init,
  .shutdown = wr_x11_shutdown,
  .poll_events = wr_x11_poll_events,
  .create_window = wr_x11_create_window,
  .destroy_window = wr_x11_destroy_window,
  .get_native_display = wr_x11_get_native_display,
  .get_native_window = wr_x11_get_native_window,
};
