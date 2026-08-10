#ifndef WR_BATCH_H
#define WR_BATCH_H

#include <stddef.h>
#include <walrus/renderer/opengl/vertex.h>
#include <stdint.h>

typedef struct WrBatch {
  WrVertex *vertices;
  uint32_t *indices;

  size_t vertex_count;
  size_t vertex_capacity;

  size_t index_count;
  size_t index_capacity;
} WrBatch;

#endif
