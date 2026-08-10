#include <walrus/renderer/opengl/opengl.h>

static int wr_opengl_init(void)
{
  return 0;
}

static void wr_opengl_shutdown(void)
{
}

static WrRenderSurface *wr_opengl_create_surface(
  WrWindow *window
)
{
  return NULL;
}

static void wr_opengl_destroy_surface(
  WrRenderSurface *surface
)
{
}

static int wr_opengl_begin_frame(
  WrRenderSurface *surface
)
{
  return 0;
}

static int wr_opengl_end_frame(
  WrRenderSurface *surface
)
{
  return 0;
}

static void wr_opengl_draw_batch(
  WrRenderSurface *surface,
  WrBatch *batch
)
{
}

WrRenderer wr_opengl_renderer = {
  .init = wr_opengl_init,
  .shutdown = wr_opengl_shutdown,

  .create_surface = wr_opengl_create_surface,
  .destroy_surface = wr_opengl_destroy_surface,

  .begin_frame = wr_opengl_begin_frame,
  .end_frame = wr_opengl_end_frame,

  .draw_batch = wr_opengl_draw_batch,
};
