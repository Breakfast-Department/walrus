#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <walrus/walrus.h>
#include <walrus/core/window.h>

int main(void)
{
  if (wr_init() != 0)
  {
    fprintf(stderr, "Failed initialize walrus\n");
    return EXIT_FAILURE;
  }

  WrWindow* window = wr_create_window("Hello, Walrus!", 800, 600);
  if (!window)
  {
    fprintf(stderr, "Failed create walrus window\n");
    wr_shutdown();
    return EXIT_FAILURE;
  }

  WrRenderSurface* surface = wr_window_get_surface(window);
  if (!surface)
  {
    fprintf(stderr, "Failed get walrus render surface\n");
    wr_window_destroy(window);
    wr_shutdown();
    return EXIT_FAILURE;
  }

  while (wr_window_should_close(window) == 0)
  {
    wr_poll_events();

    if (wr_begin_frame(surface) != 0)
      break;

    wr_clear(10.0, 10.0, 10.0, 1.0f);

    if (wr_end_frame(surface) != 0)
      break;
  }

  wr_window_destroy(window);
  wr_shutdown();
  return EXIT_SUCCESS;
}
