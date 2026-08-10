#include <stdlib.h>
#include <glad/glad.h>
#include "walrus/renderer/opengl/batch.h"
#include "walrus/renderer/opengl/egl.h"
#include <stdio.h>
#include <walrus/renderer/opengl/opengl.h>
#include <walrus/backend/backend.h>
#include <walrus/renderer/renderer.h>

static WrEGL wr_egl = {
  .display = EGL_NO_DISPLAY,
  .config = NULL,
  .context = EGL_NO_CONTEXT,
  .surface = EGL_NO_SURFACE,
  .major_version = 3,
  .minor_version = 3
};

static int wr_opengl_init(void* native_display)
{
  if (!gladLoadEGL())
  {
    fprintf(stderr, "Failed to load EGL entry points\n");
    return -1;
  }

  return wr_egl_init(&wr_egl, native_display);
}

static void wr_opengl_shutdown(void)
{
}

static WrRenderSurface *wr_opengl_create_surface(
  void *native_window
)
{
  if (!native_window)
    return NULL;

  WrRenderSurface *surface = calloc(1, sizeof(WrRenderSurface));
  if (!surface)
    return NULL;

  surface->native_window = native_window;

  if (wr_egl_create_surface(&wr_egl, native_window) < 0)
  {
    free(surface);
    return NULL;
  }

  if (wr_egl_create_context(&wr_egl) < 0)
  {
    wr_egl_destroy_surface(&wr_egl, wr_egl.surface);
    free(surface);
    return NULL;
  }

  if (wr_egl_make_current(&wr_egl, wr_egl.surface) < 0)
  {
    wr_egl_destroy_surface(&wr_egl, wr_egl.surface);
    free(surface);
    return NULL;
  }

  if (!gladLoadGLLoader((GLADloadproc)eglGetProcAddress))
  {
    fprintf(stderr, "Failed to load OpenGL functions\n");
    wr_egl_destroy_surface(&wr_egl, wr_egl.surface);
    free(surface);
    return NULL;
  }

  EGLSurface *egl_surface = calloc(1, sizeof(EGLSurface));
  if (!egl_surface)
  {
    wr_egl_destroy_surface(&wr_egl, wr_egl.surface);
    free(surface);
    return NULL;
  }

  *egl_surface = wr_egl.surface;
  surface->renderer_data = egl_surface;
  return surface;
}

static void wr_opengl_destroy_surface(
  WrRenderSurface *surface
)
{
  if (!surface)
    return;

  EGLSurface *egl_surface = surface->renderer_data;
  if (egl_surface)
  {
    wr_egl_destroy_surface(&wr_egl, *egl_surface);
    free(egl_surface);
  }

  free(surface);
}

static int wr_opengl_begin_frame(
  WrRenderSurface *surface
)
{
  if (!surface || !surface->renderer_data)
    return -1;

  EGLSurface egl_surface = *(EGLSurface*)surface->renderer_data;
  return wr_egl_make_current(&wr_egl, egl_surface);
}

static int wr_opengl_end_frame(
  WrRenderSurface *surface
)
{
  if (!surface || !surface->renderer_data)
    return -1;

  EGLSurface egl_surface = *(EGLSurface*)surface->renderer_data;
  return wr_egl_swap_buffers(&wr_egl, egl_surface);
}

static void wr_opengl_clear(float r, float g, float b, float a)
{
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

static void wr_opengl_draw_batch(
  WrRenderSurface *surface,
  WrBatch *batch
)
{
  (void)surface;
  (void)batch;
}

WrRenderer wr_opengl_renderer = {
  .init = wr_opengl_init,
  .shutdown = wr_opengl_shutdown,

  .create_surface = wr_opengl_create_surface,
  .destroy_surface = wr_opengl_destroy_surface,

  .begin_frame = wr_opengl_begin_frame,
  .end_frame = wr_opengl_end_frame,
  .clear = wr_opengl_clear,

  .draw_batch = wr_opengl_draw_batch,
};
