#include <walrus/text/text.h>
#include <walrus/renderer/opengl/batch.h>
#include <walrus/renderer/opengl/vertex.h>
#include <walrus/ui/style.h>
#include <string.h>
#include <stdlib.h>

#define WR_SHAPE_TEXT 3.0f

int wr_batch_text(
  WrBatch* batch,
  WrFont* font,
  const char* text,
  float x, float y,
  WrColor color
)
{
  return wr_batch_text_aligned(batch, font, text, x, y, color,
    WR_TEXT_ALIGN_LEFT, WR_TEXT_BASELINE_TOP);
}

int wr_batch_text_aligned(
  WrBatch* batch,
  WrFont* font,
  const char* text,
  float x, float y,
  WrColor color,
  WrTextAlign align,
  WrTextBaseline baseline
)
{
  if (!batch || !font || !text)
    return -1;

  size_t len = strlen(text);
  if (len == 0)
    return 0;

  WrFontMetrics metrics = wr_font_get_metrics(font);
  float text_width = wr_font_measure_text(font, text);

  float start_x = x;
  float start_y = y;

  switch (align) {
    case WR_TEXT_ALIGN_CENTER:
      start_x = x - text_width / 2.0f;
      break;
    case WR_TEXT_ALIGN_RIGHT:
      start_x = x - text_width;
      break;
    default:
      break;
  }

  switch (baseline) {
    case WR_TEXT_BASELINE_TOP:
      start_y = y + metrics.ascender;
      break;
    case WR_TEXT_BASELINE_MIDDLE:
      start_y = y + (metrics.ascender + metrics.descender) / 2.0f;
      break;
    case WR_TEXT_BASELINE_BOTTOM:
      start_y = y + metrics.descender;
      break;
    case WR_TEXT_BASELINE_ALPHABETIC:
    default:
      break;
  }

  float pen_x = start_x;
  float pen_y = start_y;
  uint32_t tex_id = wr_font_get_texture(font);

  wr_batch_set_font_texture(batch, tex_id);

  for (size_t i = 0; i < len; i++) {
    unsigned char c = (unsigned char)text[i];
    if (c >= 128)
      continue;

    const WrGlyph* g = wr_font_get_glyph(font, c);
    if (!g) {
      pen_x += metrics.size / 2.0f;
      continue;
    }

    float gx = pen_x + g->bearing_x;
    float gy = pen_y - g->bearing_y;
    float gw = g->width;
    float gh = g->height;

    if (gw > 0 && gh > 0) {
      size_t vbase = batch->vertex_count;

      WrVertex v0 = {
        gx, gy,
        color.r, color.g, color.b, color.a,
        g->x0, g->y0,
        (float)tex_id, 0, 0, 0,
        0.0f, WR_SHAPE_TEXT
      };
      WrVertex v1 = {
        gx + gw, gy,
        color.r, color.g, color.b, color.a,
        g->x1, g->y0,
        (float)tex_id, 0, 0, 0,
        0.0f, WR_SHAPE_TEXT
      };
      WrVertex v2 = {
        gx + gw, gy + gh,
        color.r, color.g, color.b, color.a,
        g->x1, g->y1,
        (float)tex_id, 0, 0, 0,
        0.0f, WR_SHAPE_TEXT
      };
      WrVertex v3 = {
        gx, gy + gh,
        color.r, color.g, color.b, color.a,
        g->x0, g->y1,
        (float)tex_id, 0, 0, 0,
        0.0f, WR_SHAPE_TEXT
      };

      /* Ensure capacity */
      if (batch->vertex_count + 4 > batch->vertex_capacity) {
        size_t newcap = batch->vertex_capacity ? batch->vertex_capacity * 2 : 64;
        while (newcap < batch->vertex_count + 4) newcap *= 2;
        WrVertex* nv = realloc(batch->vertices, sizeof(WrVertex) * newcap);
        if (!nv) return -1;
        batch->vertices = nv;
        batch->vertex_capacity = newcap;
      }

      if (batch->index_count + 6 > batch->index_capacity) {
        size_t newcap = batch->index_capacity ? batch->index_capacity * 2 : 128;
        while (newcap < batch->index_count + 6) newcap *= 2;
        uint32_t* ni = realloc(batch->indices, sizeof(uint32_t) * newcap);
        if (!ni) return -1;
        batch->indices = ni;
        batch->index_capacity = newcap;
      }

      batch->vertices[batch->vertex_count++] = v0;
      batch->vertices[batch->vertex_count++] = v1;
      batch->vertices[batch->vertex_count++] = v2;
      batch->vertices[batch->vertex_count++] = v3;

      batch->indices[batch->index_count++] = (uint32_t)(vbase + 0);
      batch->indices[batch->index_count++] = (uint32_t)(vbase + 1);
      batch->indices[batch->index_count++] = (uint32_t)(vbase + 2);
      batch->indices[batch->index_count++] = (uint32_t)(vbase + 2);
      batch->indices[batch->index_count++] = (uint32_t)(vbase + 3);
      batch->indices[batch->index_count++] = (uint32_t)(vbase + 0);
    }

    pen_x += g->advance;
  }

  return 0;
}
