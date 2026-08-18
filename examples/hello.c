#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <walrus/walrus.h>
#include <walrus/core/window.h>
#include <walrus/ui/widget.h>
#include <walrus/ui/style.h>

#include <math.h>
#include <signal.h>

static volatile sig_atomic_t g_interrupt_requested = 0;

static void handle_sigint(int sig)
{
  (void)sig;
  g_interrupt_requested = 1;
}

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

  /* Create a simple widget and add it to the window */
  WrWidget* widget = wr_create_widget("my-id/my-class/HelloWidget");
  WrBackground bg_style = { .type = WR_BACKGROUND_COLOR, .color = { 0.6f, 0.6f, 0.8f, 1.0f } };
  widget->style.background = bg_style;
  widget->style.layout.width = 200.0f;
  widget->style.layout.height = 100.0f;
  widget->style.border = (WrBorder){ .radius = 10.0f };

  WrElement el = { .type = WR_ELEMENT_TYPE_WIDGET, .data = widget };
  wr_window_add_child(window, &el);

  int count = 0;
  signal(SIGINT, handle_sigint);
  while (wr_window_should_close(window) == 0)
  {
    if (g_interrupt_requested) { wr_window_set_should_close(window, 1); break; }
    count++;

    float t = (float)count * 0.02f;

    widget->style.background.type = WR_BACKGROUND_COLOR;

    widget->style.background.color.r =
        0.5f + 0.5f * sinf(t);

    widget->style.background.color.g =
        0.5f + 0.5f * sinf(t + 2.094f);

    widget->style.background.color.b =
        0.5f + 0.5f * sinf(t + 4.188f);

    widget->style.background.color.a = 1.0f;

    wr_poll_events();

    wr_render();
  }

  wr_window_destroy(window);
  wr_shutdown();
  return EXIT_SUCCESS;
}
