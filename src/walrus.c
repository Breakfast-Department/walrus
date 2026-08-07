#include "walrus/walrus.h"
#include "walrus/window.h"

static wr_backend_t *backend = NULL;

void wr_init() {
  backend = wr_backend_get();
  backend->init();
}

wr_backend_t *wr_get_backend() {
  return backend;
}

void wr_shutdown()
{
  backend->shutdown();
}
