#ifndef WR_WINDOW_H
#define WR_WINDOW_H

typedef struct WrWindow WrWindow;

typedef struct WrRenderSurface WrRenderSurface;

WrWindow* wr_create_window(char* title, int width, int height);
void wr_window_destroy(WrWindow* window);
WrRenderSurface* wr_window_get_surface(WrWindow* window);

int wr_window_should_close(WrWindow* window);
void wr_window_set_should_close(WrWindow* window, int value);

#endif
