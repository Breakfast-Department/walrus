// backend.c

#include "walrus/backend/backend.h"
#include "walrus/backend/wayland/wayland.h"
#include "walrus/backend/x11/x11.h"
#include <stdlib.h>

wr_backend_t *wr_backend_get(void)
{
  if (getenv("WAYLAND_DISPLAY"))
  {
    return &wl_backend;
  }

  if (getenv("DISPLAY"))
  {
    return &x11_backend;
  }

  return NULL;
}
