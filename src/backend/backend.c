#include "walrus/backend/backend.h"
#include "walrus/backend/wayland/wayland.h"

#include <stdio.h>
#include <stdlib.h>

static WrBackend* wr_backend = NULL;

WrBackend* wr_backend_init(void)
{
  if (getenv("WAYLAND_DISPLAY"))
  {
    wr_backend = &wr_wayland_backend;
  }
  else if (getenv("DISPLAY"))
  {
    // todo -> return wr_x11_backend
  }

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
