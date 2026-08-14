#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <walrus/walrus.h>
#include <walrus/core/window.h>
#include <walrus/ui/widget.h>
#include <walrus/ui/style.h>

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

  /* Create a simple widget and draw it every frame */
  WrWidget* widget = wr_create_widget("my-id/my-class/HelloWidget");
  WrBackground bg_style = { .type = WR_BACKGROUND_COLOR, .color = { 0.6f, 0.6f, 0.8f, 1.0f } };
  widget->style.background = bg_style;
  widget->style.layout.width = 200.0f;
  widget->style.layout.height = 100.0f;
  widget->style.border = (WrBorder){ .radius = 10.0f };

  while (wr_window_should_close(window) == 0)
  {
    wr_poll_events();

    if (wr_begin_frame(surface) != 0)
      break;

    wr_clear(10.0, 10.0, 10.0, 1.0f);

    wr_window_draw_widget(window, widget);

    if (wr_end_frame(surface) != 0)
      break;
  }

  wr_window_destroy(window);
  wr_shutdown();
  return EXIT_SUCCESS;
}
