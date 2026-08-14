#ifndef WR_RENDERER_BATCH_H
#define WR_RENDERER_BATCH_H

#include <walrus/renderer/renderer.h>
#include <walrus/ui/style.h>

typedef struct WrBatch WrBatch;

WrBatch* wr_batch_create(void);
void wr_batch_destroy(WrBatch* batch);

/* Add a filled rectangle to the batch. Coordinates in pixels. */
int wr_batch_rect(
  WrBatch* batch,
  float x, float y,
  float width, float height,
  WrColor color
);

#endif
