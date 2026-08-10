#include "walrus/renderer/opengl/opengl.h"
#include <walrus/renderer/renderer.h>
#include <walrus/renderer/opengl/batch.h>

static WrRenderer* wr_renderer = &wr_opengl_renderer;

WrRenderer* wr_renderer_init(void) {
  return wr_renderer;
}

WrRenderer* wr_get_renderer(void) {
  return wr_renderer;
}
