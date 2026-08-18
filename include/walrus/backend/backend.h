#ifndef WR_BACKEND_H
#define WR_BACKEND_H

typedef struct WrBackend {
  int (*init)(void);
  void (*shutdown)(void);

  void (*poll_events)(void);

  void* (*create_window)(void*, char*, int, int);
  void (*destroy_window)(void*);

  void* (*get_native_display)(void);
  void* (*get_native_window)(void*);

  /* Resize the native window */
  void (*resize_window)(void*, int, int);

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
