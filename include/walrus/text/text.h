#ifndef WR_TEXT_H
#define WR_TEXT_H

#include <walrus/text/font.h>
#include <walrus/renderer/batch.h>

typedef struct WrColor WrColor;

/* Text alignment */
typedef enum WrTextAlign {
  WR_TEXT_ALIGN_LEFT,
  WR_TEXT_ALIGN_CENTER,
  WR_TEXT_ALIGN_RIGHT
} WrTextAlign;

/* Text baseline */
typedef enum WrTextBaseline {
  WR_TEXT_BASELINE_TOP,
  WR_TEXT_BASELINE_MIDDLE,
  WR_TEXT_BASELINE_BOTTOM,
  WR_TEXT_BASELINE_ALPHABETIC
} WrTextBaseline;

/* Add text to a batch */
int wr_batch_text(
  WrBatch* batch,
  WrFont* font,
  const char* text,
  float x, float y,
  WrColor color
);

/* Add text with alignment */
int wr_batch_text_aligned(
  WrBatch* batch,
  WrFont* font,
  const char* text,
  float x, float y,
  WrColor color,
  WrTextAlign align,
  WrTextBaseline baseline
);

#endif
