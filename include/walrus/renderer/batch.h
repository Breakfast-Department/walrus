#ifndef WR_RENDERER_BATCH_H
#define WR_RENDERER_BATCH_H

#include <stdint.h>
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

/* Add a filled rounded rectangle to the batch. */
int wr_batch_rounded_rect(
  WrBatch* batch,
  float x, float y,
  float width, float height,
  float radius,
  WrColor color
);

/* Add a filled circle to the batch. */
int wr_batch_circle(
  WrBatch* batch,
  float cx, float cy,
  float radius,
  WrColor color
);

/* Set the font texture for text rendering */
void wr_batch_set_font_texture(WrBatch* batch, uint32_t texture);

#endif
