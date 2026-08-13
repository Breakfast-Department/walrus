#ifndef WR_WINDOW_H
#define WR_WINDOW_H

typedef struct WrWindow WrWindow;

/* Forward declare widget type so other headers can reference it. */
typedef struct WrWidget WrWidget;

typedef struct WrRenderSurface WrRenderSurface;

WrWindow* wr_create_window(char* title, int width, int height);
void wr_window_destroy(WrWindow* window);
WrRenderSurface* wr_window_get_surface(WrWindow* window);

int wr_window_should_close(WrWindow* window);
void wr_window_set_should_close(WrWindow* window, int value);

/* Draw an element/widget on the window. The widget's `render` callback
	will be invoked with the current window surface. */
void wr_window_draw_widget(WrWindow* window, struct WrWidget* widget);

#endif
