#ifndef WR_WINDOW_H
#define WR_WINDOW_H

typedef struct WrWindow WrWindow;

/* Forward declare widget type so other headers can reference it. */
typedef struct WrWidget WrWidget;

typedef struct WrRenderSurface WrRenderSurface;
typedef struct WrElement WrElement;

#include <walrus/core/input.h>

WrWindow* wr_create_window(char* title, int width, int height);
void wr_window_destroy(WrWindow* window);
WrRenderSurface* wr_window_get_surface(WrWindow* window);

int wr_window_should_close(WrWindow* window);
void wr_window_set_should_close(WrWindow* window, int value);

/* Draw an element/widget on the window. The widget's `render` callback
	will be invoked with the current window surface. */
void wr_window_draw_widget(WrWindow* window, WrWidget* widget);

/* Resize the window target surface. This updates the stored width/height
	and forwards the resize to the backend if available. */
void wr_window_set_size(WrWindow* window, int width, int height);

/* Handle an input event for the given window (e.g. resize events). */
void wr_window_handle_input(WrWindow* window, WrInputEvent* ev);

/* Query current window size. Returns 0 on success. */
int wr_window_get_size(WrWindow* window, int* width, int* height);

/* Check if window is currently maximized. */
int wr_window_is_maximized(void);

/* Manage child elements in content area */
int wr_window_add_child(WrWindow* window, WrElement* element);
int wr_window_remove_child(WrWindow* window, WrElement* element);

/* Manage child elements in window decoration (title bar) */
int wr_window_add_decoration(WrWindow* window, WrElement* element);
int wr_window_remove_decoration(WrWindow* window, WrElement* element);

/* Get content area bounds (where child widgets should render) */
void wr_window_get_content_bounds(WrWindow* window, float* x, float* y, float* w, float* h);

#endif
