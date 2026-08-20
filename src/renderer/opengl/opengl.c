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
  if (wr_egl_make_current(&wr_egl, egl_surface) < 0)
    return -1;

  EGLint width, height;
  eglQuerySurface(wr_egl.display, egl_surface, EGL_WIDTH, &width);
  eglQuerySurface(wr_egl.display, egl_surface, EGL_HEIGHT, &height);
  glViewport(0, 0, width, height);

  return 0;
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
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_SCISSOR_TEST);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
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
  static GLint u_viewport = -1;
  static GLint u_texture = -1;

  if (prog == 0)
  {
    const char *vsrc =
      "#version 330 core\n"
      "layout(location = 0) in vec2 aPos;\n"
      "layout(location = 1) in vec4 aColor;\n"
      "layout(location = 2) in vec2 aUV;\n"
      "layout(location = 3) in vec4 aRect;\n"
      "layout(location = 4) in float aRadius;\n"
      "layout(location = 5) in float aShapeType;\n"
      "uniform vec2 uViewport;\n"
      "out vec4 vColor;\n"
      "out vec2 vUV;\n"
      "out vec2 vPixelPos;\n"
      "out vec4 vRect;\n"
      "out float vRadius;\n"
      "out float vShapeType;\n"
      "void main() {\n"
      "  vColor = aColor;\n"
      "  vUV = aUV;\n"
      "  vPixelPos = aPos;\n"
      "  vRect = aRect;\n"
      "  vRadius = aRadius;\n"
      "  vShapeType = aShapeType;\n"
      "  vec2 ndc = (aPos / uViewport) * 2.0 - 1.0;\n"
      "  ndc.y = -ndc.y;\n"
      "  gl_Position = vec4(ndc, 0.0, 1.0);\n"
      "}\n";

    const char *fsrc =
      "#version 330 core\n"
      "in vec4 vColor;\n"
      "in vec2 vUV;\n"
      "in vec2 vPixelPos;\n"
      "in vec4 vRect;\n"
      "in float vRadius;\n"
      "in float vShapeType;\n"
      "uniform sampler2D uTexture;\n"
      "out vec4 FragColor;\n"
      "float sdf_rounded_rect(vec2 p, vec2 center, vec2 half_size, float r) {\n"
      "  vec2 d = abs(p - center) - half_size + r;\n"
      "  return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;\n"
      "}\n"
      "float sdf_circle(vec2 p, vec2 center, float r) {\n"
      "  return length(p - center) - r;\n"
      "}\n"
      "void main() {\n"
      "  float alpha = vColor.a;\n"
      "  if (vShapeType > 2.5) {\n"
      "    float texAlpha = texture(uTexture, vUV).r;\n"
      "    alpha *= texAlpha;\n"
      "  } else if (vShapeType > 0.5) {\n"
      "    vec2 center = vRect.xy + vRect.zw * 0.5;\n"
      "    float d;\n"
      "    if (vShapeType < 1.5) {\n"
      "      vec2 half_size = vRect.zw * 0.5;\n"
      "      d = sdf_rounded_rect(vPixelPos, center, half_size, vRadius);\n"
      "    } else {\n"
      "      d = sdf_circle(vPixelPos, center, vRadius);\n"
      "    }\n"
      "    float aa = 1.0;\n"
      "    alpha *= 1.0 - smoothstep(-aa, aa, d);\n"
      "  }\n"
      "  FragColor = vec4(vColor.rgb * alpha, alpha);\n"
      "}\n";

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

    u_viewport = glGetUniformLocation(prog, "uViewport");
    u_texture = glGetUniformLocation(prog, "uTexture");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
  }

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, batch->vertex_count * sizeof(WrVertex), batch->vertices, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, batch->index_count * sizeof(uint32_t), batch->indices, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, x));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, r));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, u));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, rect_x));
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, radius));
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(WrVertex), (void*)offsetof(WrVertex, shape_type));

  glUseProgram(prog);
  glUniform2f(u_viewport, (float)viewport[2], (float)viewport[3]);
  glUniform1i(u_texture, 0);

  if (batch->font_texture) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, batch->font_texture);
  }

  glDrawElements(GL_TRIANGLES, (GLsizei)batch->index_count, GL_UNSIGNED_INT, 0);

  if (batch->font_texture) {
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glBindVertexArray(0);
  glDisable(GL_BLEND);
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
