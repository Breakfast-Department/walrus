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

  /* Create a widget in the content area */
  WrWidget* content_widget = wr_create_widget("content-panel");
  content_widget->style.background = (WrBackground){
    .type = WR_BACKGROUND_COLOR,
    .color = { 0.3f, 0.5f, 0.7f, 1.0f }
  };
  content_widget->style.layout.width = 200.0f;
  content_widget->style.layout.height = 100.0f;
  content_widget->style.border.radius = 8.0f;

  WrElement content_el = { .type = WR_ELEMENT_TYPE_WIDGET, .data = content_widget };
  wr_window_add_child(window, &content_el);

  /* Create a button widget in the decoration/titlebar area */
  WrWidget* menu_btn = wr_create_widget("menu-button");
  menu_btn->style.background = (WrBackground){
    .type = WR_BACKGROUND_COLOR,
    .color = { 0.5f, 0.5f, 0.55f, 1.0f }
  };
  menu_btn->style.layout.width = 24.0f;
  menu_btn->style.layout.height = 24.0f;
  menu_btn->style.layout.margin.left = 12.0f;
  menu_btn->style.layout.margin.top = 4.0f;
  menu_btn->style.border.radius = 4.0f;

  WrElement deco_el = { .type = WR_ELEMENT_TYPE_WIDGET, .data = menu_btn };
  wr_window_add_decoration(window, &deco_el);

  signal(SIGINT, handle_sigint);
  while (wr_window_should_close(window) == 0)
  {
    if (g_interrupt_requested) { wr_window_set_should_close(window, 1); break; }
    wr_poll_events();
    wr_render();
  }

  wr_window_destroy(window);
  wr_shutdown();
  return EXIT_SUCCESS;
}
