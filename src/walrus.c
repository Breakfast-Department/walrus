#include "walrus/walrus.h"
#include "walrus/core/window.h"

static wr_backend_t *backend = NULL;

void wr_init() {
  backend = wr_backend_get();
}

wr_backend_t *wr_get_backend() {
  return backend;
}
