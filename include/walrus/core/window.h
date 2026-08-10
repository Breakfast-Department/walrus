#ifndef WR_WINDOW_H
#define WR_WINDOW_H

typedef struct WrWindow WrWindow;

WrWindow* wr_create_window(char* title, int width, int height);
void wr_window_destroy(WrWindow* window);

#endif
