#include "walrus/backend/x11/x11.h"

#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

static Display *display = NULL;

static int x11_init(void)
{
  display = XOpenDisplay(NULL);

  if (!display)
  {
    fprintf(stderr, "Failed to connect to X11 server\n");
    return -1;
  }

  printf("Connected to X11\n");

  return 0;
}


static void x11_shutdown(void)
{
  if (display)
  {
    XCloseDisplay(display);
    display = NULL;
  }

  printf("Disconnected from X11\n");
}


static void x11_create_window(wr_window_t *window)
{
  if (!display)
  {
    fprintf(stderr, "X11 is not initialized\n");
    return;
  }

  /*
    Nanti di sini:
    - DefaultScreen()
    - RootWindow()
    - XCreateSimpleWindow()
    - XMapWindow()
    - XFlush()
  */

  printf("Create X11 window\n");
}

static void x11_destroy_window(wr_window_t *window)
{

}

wr_backend_t x11_backend = {
  .type = WR_BACKEND_X11,
  .init = x11_init,
  .shutdown = x11_shutdown,
  .create_window = x11_create_window,
  .destroy_window = x11_destroy_window,
};
