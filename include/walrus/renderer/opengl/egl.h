#ifndef WR_EGL_H
#define WR_EGL_H

#include <glad/egl.h>

typedef struct WrEGL {
  EGLDisplay display;
  EGLConfig config;
  EGLContext context;
  EGLSurface surface;

  int major_version;
  int minor_version;
} WrEGL;

int wr_egl_init(
  WrEGL* egl,
  void* native_display
);

int wr_egl_create_surface(
  WrEGL* egl,
  void* native_window
);

int wr_egl_create_context(
  WrEGL* egl
);

int wr_egl_make_current(
  WrEGL* egl,
  EGLSurface surface
);

int wr_egl_swap_buffers(
  WrEGL* egl,
  EGLSurface surface
);

void wr_egl_destroy_surface(
  WrEGL* egl,
  EGLSurface surface
);

void wr_egl_destroy(
  WrEGL* egl
);

int wr_egl_make_current(
  WrEGL* egl,
  EGLSurface surface
);

int wr_egl_swap_buffers(
  WrEGL* egl,
  EGLSurface surface
);

void wr_egl_destroy(
  WrEGL* egl
);

#endif
