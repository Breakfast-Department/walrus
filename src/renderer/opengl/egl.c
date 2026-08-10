#include <walrus/renderer/opengl/egl.h>

#include <EGL/egl.h>
#include <stdio.h>
#include <string.h>

static void wr_egl_reset(WrEGL *egl)
{
  egl->display = EGL_NO_DISPLAY;
  egl->config = NULL;
  egl->context = EGL_NO_CONTEXT;
  egl->surface = EGL_NO_SURFACE;

  egl->major_version = 0;
  egl->minor_version = 0;
}

int wr_egl_init(WrEGL *egl, void *native_display)
{
  if (!egl || !native_display)
  {
    fprintf(stderr, "Egl or native display is null\n");
    return -1;
  }

  wr_egl_reset(egl);

  if (!gladLoadEGL())
  {
    fprintf(stderr, "Failed to load EGL entry points\n");
    return -1;
  }

  egl->display = eglGetDisplay(
    (EGLNativeDisplayType)native_display
  );

  if (egl->display == EGL_NO_DISPLAY)
  {
    fprintf(stderr, "Failed to get display\n");
    return -1;
  }

  if (!eglInitialize(
    egl->display,
    &egl->major_version,
    &egl->minor_version
  ))
  {
    fprintf(stderr, "Failed to initialize\n");

    wr_egl_reset(egl);
    return -1;
  }

  /*
    * Kita akan menggunakan OpenGL, bukan OpenGL ES.
    */
  if (!eglBindAPI(EGL_OPENGL_API))
  {
    fprintf(stderr, "Failed to bind OpenGL API\n");

    eglTerminate(egl->display);
    wr_egl_reset(egl);

    return -1;
  }

  return 0;
}

int wr_egl_create_surface(
    WrEGL *egl,
    void *native_window
)
{
  if (!egl || !native_window)
    return -1;

  if (egl->display == EGL_NO_DISPLAY)
    return -1;

  const EGLint attributes[] = {
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,

    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,

    EGL_NONE
  };

  EGLint num_configs = 0;

  if (!eglChooseConfig(
    egl->display,
    attributes,
    &egl->config,
    1,
    &num_configs
  ))
  {
    fprintf(stderr, "EGL: failed to choose config\n");
    return -1;
  }

  if (num_configs == 0 || !egl->config)
  {
    fprintf(stderr, "EGL: no suitable config found\n");
    return -1;
  }

  egl->surface = eglCreateWindowSurface(
    egl->display,
    egl->config,
    (EGLNativeWindowType)native_window,
    NULL
  );

  if (egl->surface == EGL_NO_SURFACE)
  {
    fprintf(stderr, "EGL: failed to create window surface\n");
    return -1;
  }

  return 0;
}

int wr_egl_create_context(WrEGL *egl)
{
  if (!egl)
    return -1;

  if (egl->display == EGL_NO_DISPLAY)
    return -1;

  if (!egl->config)
    return -1;

  const EGLint attributes[] = {
    EGL_NONE
  };

  egl->context = eglCreateContext(
    egl->display,
    egl->config,
    EGL_NO_CONTEXT,
    attributes
  );

  if (egl->context == EGL_NO_CONTEXT)
  {
    fprintf(stderr, "EGL: failed to create context\n");
    return -1;
  }

  return 0;
}

int wr_egl_make_current(
  WrEGL *egl,
  EGLSurface surface
)
{
  if (!egl)
    return -1;

  if (egl->display == EGL_NO_DISPLAY)
    return -1;

  if (egl->context == EGL_NO_CONTEXT)
    return -1;

  if (surface == EGL_NO_SURFACE)
    return -1;

  if (!eglMakeCurrent(
    egl->display,
    surface,
    surface,
    egl->context
  ))
  {
    fprintf(stderr, "EGL: failed to make context current\n");
    return -1;
  }

  return 0;
}

int wr_egl_swap_buffers(
  WrEGL *egl,
  EGLSurface surface
)
{
  if (!egl)
    return -1;

  if (egl->display == EGL_NO_DISPLAY)
    return -1;

  if (surface == EGL_NO_SURFACE)
    return -1;

  if (!eglSwapBuffers(
    egl->display,
    surface
  ))
  {
    fprintf(stderr, "EGL: failed to swap buffers\n");
    return -1;
  }

  return 0;
}

void wr_egl_destroy_surface(
  WrEGL *egl,
  EGLSurface surface
)
{
  if (!egl)
    return;

  if (egl->display == EGL_NO_DISPLAY)
    return;

  if (surface != EGL_NO_SURFACE)
  {
    eglDestroySurface(
      egl->display,
      surface
    );
  }

  if (egl->surface == surface)
    egl->surface = EGL_NO_SURFACE;
}

void wr_egl_destroy(WrEGL *egl)
{
  if (!egl)
    return;

  if (egl->display != EGL_NO_DISPLAY)
  {
    if (egl->surface != EGL_NO_SURFACE)
    {
      eglDestroySurface(
        egl->display,
        egl->surface
      );
      egl->surface = EGL_NO_SURFACE;
    }

    if (egl->context != EGL_NO_CONTEXT)
    {
      eglDestroyContext(
        egl->display,
        egl->context
      );
    }

    eglTerminate(egl->display);
  }

  wr_egl_reset(egl);
}
