#include <walrus/backend/x11/x11.h>
#include <stddef.h>

static int wr_x11_init(void)
{
  return 0;
}

static void wr_x11_shutdown(void)
{
}

static void wr_x11_poll_events(void)
{
}

static void* wr_x11_create_window(void* native_display, char* title, int width, int height)
{
  (void)native_display;
  (void)title;
  (void)width;
  (void)height;
  return NULL;
}

static void wr_x11_destroy_window(void* window)
{
  (void)window;
}

static void* wr_x11_get_native_display(void)
{
  return NULL;
}

static void* wr_x11_get_native_window(void* window)
{
  (void)window;
  return NULL;
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
