#ifndef WALRUS_WINDOW_H
#define WALRUS_WINDOW_H

#include "../backend/backend.h"
#include <stddef.h>

typedef struct wr_window {

    char *title;

    int width;
    int height;

    int should_close;

    void *backend_data;

} wr_window_t;

wr_window_t *wr_create_window(
    const char *title,
    int width,
    int height
);

void wr_window_destroy(
    wr_window_t *window
);

#endif
