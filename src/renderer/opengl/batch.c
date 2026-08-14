#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <walrus/renderer/batch.h>
#include "walrus/renderer/opengl/batch.h"
#include "walrus/renderer/opengl/vertex.h"

WrBatch* wr_batch_create(void)
{
  WrBatch* b = calloc(1, sizeof(WrBatch));
  if (!b)
    return NULL;
  return b;
}

void wr_batch_destroy(WrBatch* batch)
{
  if (!batch)
    return;
  free(batch->vertices);
  free(batch->indices);
  free(batch);
}

static int ensure_vertices(WrBatch* b, size_t need)
{
  if (b->vertex_capacity >= need)
    return 0;
  size_t newcap = b->vertex_capacity ? b->vertex_capacity * 2 : 8;
  while (newcap < need) newcap *= 2;
  WrVertex* nv = realloc(b->vertices, sizeof(WrVertex) * newcap);
  if (!nv) return -1;
  b->vertices = nv;
  b->vertex_capacity = newcap;
  return 0;
}

static int ensure_indices(WrBatch* b, size_t need)
{
  if (b->index_capacity >= need)
    return 0;
  size_t newcap = b->index_capacity ? b->index_capacity * 2 : 16;
  while (newcap < need) newcap *= 2;
  uint32_t* ni = realloc(b->indices, sizeof(uint32_t) * newcap);
  if (!ni) return -1;
  b->indices = ni;
  b->index_capacity = newcap;
  return 0;
}

int wr_batch_rect(
  WrBatch* batch,
  float x, float y,
  float width, float height,
  WrColor color
)
{
  if (!batch)
    return -1;

  /* 4 vertices, 6 indices */
  if (ensure_vertices(batch, batch->vertex_count + 4) < 0)
    return -1;
  if (ensure_indices(batch, batch->index_count + 6) < 0)
    return -1;

  size_t vbase = batch->vertex_count;

  WrVertex v0 = { x, y, color.r, color.g, color.b, color.a, 0.0f, 0.0f };
  WrVertex v1 = { x + width, y, color.r, color.g, color.b, color.a, 1.0f, 0.0f };
  WrVertex v2 = { x + width, y + height, color.r, color.g, color.b, color.a, 1.0f, 1.0f };
  WrVertex v3 = { x, y + height, color.r, color.g, color.b, color.a, 0.0f, 1.0f };

  batch->vertices[batch->vertex_count++] = v0;
  batch->vertices[batch->vertex_count++] = v1;
  batch->vertices[batch->vertex_count++] = v2;
  batch->vertices[batch->vertex_count++] = v3;

  uint32_t i0 = vbase + 0;
  uint32_t i1 = vbase + 1;
  uint32_t i2 = vbase + 2;
  uint32_t i3 = vbase + 3;

  batch->indices[batch->index_count++] = i0;
  batch->indices[batch->index_count++] = i1;
  batch->indices[batch->index_count++] = i2;
  batch->indices[batch->index_count++] = i2;
  batch->indices[batch->index_count++] = i3;
  batch->indices[batch->index_count++] = i0;

  return 0;
}
