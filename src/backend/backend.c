#include "walrus/backend/backend.h"

#if WALRUS_ENABLE_WAYLAND
#include "walrus/backend/wayland/wayland.h"
#endif

#if WALRUS_ENABLE_X11
#include "walrus/backend/x11/x11.h"
#endif

#include <stdio.h>
#include <stdlib.h>

static WrBackend* wr_backend = NULL;

WrBackend* wr_backend_init(void)
{
#if WALRUS_ENABLE_WAYLAND
  if (getenv("WAYLAND_DISPLAY"))
  {
    wr_backend = &wr_wayland_backend;
  }
#endif

#if WALRUS_ENABLE_X11
  if (!wr_backend && getenv("DISPLAY"))
  {
    wr_backend = &wr_x11_backend;
  }
#endif

  if (!wr_backend)
  {
    fprintf(stderr, "Failed initialize backend\n");
    return NULL;
  }

  return wr_backend;
}

WrBackend* wr_get_backend(void)
{
  return wr_backend;
}
