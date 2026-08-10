#ifndef WR_CONFIG_H
#define WR_CONFIG_H

#include "walrus/renderer/opengl/egl.h"
#include <stdio.h>

WrEGL wr_egl = {
  .display = EGL_NO_DISPLAY,
  .config = NULL,
  .context = EGL_NO_CONTEXT,

  .major_version = 3,
  .minor_version = 3
};

#endif
