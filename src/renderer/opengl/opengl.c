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
  if (!surface || !batch || batch->vertex_count == 0 || batch->index_count == 0)
    return;

  static GLuint prog = 0;
  static GLuint vao = 0;
  static GLuint vbo = 0;
  static GLuint ebo = 0;

  if (prog == 0)
  {
    const char *vsrc =
      "#version 330 core\n"
      "layout(location = 0) in vec2 aPos;\n"
      "layout(location = 1) in vec4 aColor;\n"
      "layout(location = 2) in vec2 aUV;\n"
      "out vec4 vColor;\n"
      "void main() { vColor = aColor; gl_Position = vec4(aPos, 0.0, 1.0); }\n";

    const char *fsrc =
      "#version 330 core\n"
      "in vec4 vColor;\n"
      "out vec4 FragColor;\n"
      "void main() { FragColor = vColor; }\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, NULL);
    glCompileShader(fs);

    prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
  }

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, batch->vertex_count * sizeof(WrVertex), batch->vertices, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch->index_count * sizeof(uint32_t), batch->indices, GL_DYNAMIC_DRAW);

  /* vertex layout: x,y (2), r,g,b,a (4), u,v (2) */
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, x));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, r));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, u));

  glUseProgram(prog);
  glDrawElements(GL_TRIANGLES, (GLsizei)batch->index_count, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
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
