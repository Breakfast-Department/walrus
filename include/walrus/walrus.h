#ifndef WALRUS_H
#define WALRUS_H

#include "backend/backend.h"

void wr_init();
wr_backend_t *wr_get_backend();

void wr_shutdown();

#endif
