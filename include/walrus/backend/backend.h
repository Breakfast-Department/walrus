#ifndef WR_BACKEND_H
#define WR_BACKEND_H

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

typedef struct WrBackend {
  int (*init)(void);
  void (*shutdown)(void);

  void (*poll_events)(void);

  void* (*create_window)(char*);
  void (*destroy_window)(void*);

  // void (*show_window)(void);
  // void (*hide_window)(void);

  // void (*set_title)(void);
  // void (*set_size)(void);

  // void (*request_redraw)(void);
  // void (*get_window_size)(void);

} WrBackend;

WrBackend* wr_backend_init(void);
WrBackend* wr_get_backend(void);

#endif
