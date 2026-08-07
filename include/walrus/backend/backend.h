#ifndef WALRUS_BACKEND_H
#define WALRUS_BACKEND_H

#include "../window.h"

typedef enum {
    WR_BACKEND_WAYLAND,
    WR_BACKEND_X11
} wr_backend_type;

typedef struct {
  wr_backend_type type;
  int (*init)(void);
  void (*poll_events)(void);

  void (*shutdown)(void);

  void (*create_window)(wr_window_t *window);
  void (*destroy_window)(wr_window_t *window);
} wr_backend_t;

wr_backend_t *wr_backend_get();

#endif
