#ifndef WR_FONT_H
#define WR_FONT_H

#include <stddef.h>
#include <stdint.h>

typedef struct WrFont WrFont;

typedef struct WrGlyph {
  uint32_t codepoint;
  float x0, y0, x1, y1;   /* texture coordinates */
  float width, height;     /* glyph size in pixels */
  float bearing_x, bearing_y;
  float advance;
} WrGlyph;

typedef struct WrFontMetrics {
  float ascender;
  float descender;
  float line_height;
  float size;
} WrFontMetrics;

/* Initialize the font subsystem */
int wr_font_init(void);

/* Shutdown the font subsystem */
void wr_font_shutdown(void);

/* Load a font from file */
WrFont* wr_font_load(const char* path, float size);

/* Load a font from memory */
WrFont* wr_font_load_memory(const unsigned char* data, size_t size, float pixel_size);

/* Destroy a font */
void wr_font_destroy(WrFont* font);

/* Get font metrics */
WrFontMetrics wr_font_get_metrics(WrFont* font);

/* Get glyph for a codepoint */
const WrGlyph* wr_font_get_glyph(WrFont* font, uint32_t codepoint);

/* Get the font atlas texture ID (OpenGL) */
uint32_t wr_font_get_texture(WrFont* font);

/* Get atlas dimensions */
void wr_font_get_atlas_size(WrFont* font, int* width, int* height);

/* Measure text width */
float wr_font_measure_text(WrFont* font, const char* text);

/* Measure text with length */
float wr_font_measure_text_n(WrFont* font, const char* text, size_t len);

#endif
