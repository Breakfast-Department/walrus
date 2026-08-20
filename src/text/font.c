#include <walrus/text/font.h>
#include <glad/glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define WR_ATLAS_SIZE 1024
#define WR_GLYPH_PADDING 2
#define WR_ASCII_START 32
#define WR_ASCII_END 127

static FT_Library g_ft_library = NULL;

struct WrFont {
  FT_Face face;
  float size;

  GLuint texture;
  int atlas_width;
  int atlas_height;

  WrGlyph glyphs[128];
  WrFontMetrics metrics;
};

int wr_font_init(void)
{
  if (g_ft_library)
    return 0;

  if (FT_Init_FreeType(&g_ft_library) != 0) {
    fprintf(stderr, "Failed to initialize FreeType\n");
    return -1;
  }

  return 0;
}

void wr_font_shutdown(void)
{
  if (g_ft_library) {
    FT_Done_FreeType(g_ft_library);
    g_ft_library = NULL;
  }
}

static int build_atlas(WrFont* font)
{
  FT_Face face = font->face;

  int atlas_width = WR_ATLAS_SIZE;
  int atlas_height = WR_ATLAS_SIZE;

  unsigned char* atlas_data = calloc(1, atlas_width * atlas_height);
  if (!atlas_data)
    return -1;

  int pen_x = WR_GLYPH_PADDING;
  int pen_y = WR_GLYPH_PADDING;
  int row_height = 0;

  for (uint32_t c = WR_ASCII_START; c < WR_ASCII_END; c++) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
      continue;

    FT_GlyphSlot g = face->glyph;
    int gw = (int)g->bitmap.width;
    int gh = (int)g->bitmap.rows;

    if (pen_x + gw + WR_GLYPH_PADDING > atlas_width) {
      pen_x = WR_GLYPH_PADDING;
      pen_y += row_height + WR_GLYPH_PADDING;
      row_height = 0;
    }

    if (pen_y + gh + WR_GLYPH_PADDING > atlas_height) {
      fprintf(stderr, "Font atlas overflow\n");
      break;
    }

    for (int y = 0; y < gh; y++) {
      for (int x = 0; x < gw; x++) {
        int dst_idx = (pen_y + y) * atlas_width + (pen_x + x);
        int src_idx = y * (int)g->bitmap.pitch + x;
        atlas_data[dst_idx] = g->bitmap.buffer[src_idx];
      }
    }

    WrGlyph* glyph = &font->glyphs[c];
    glyph->codepoint = c;
    glyph->x0 = (float)pen_x / (float)atlas_width;
    glyph->y0 = (float)pen_y / (float)atlas_height;
    glyph->x1 = (float)(pen_x + gw) / (float)atlas_width;
    glyph->y1 = (float)(pen_y + gh) / (float)atlas_height;
    glyph->width = (float)gw;
    glyph->height = (float)gh;
    glyph->bearing_x = (float)g->bitmap_left;
    glyph->bearing_y = (float)g->bitmap_top;
    glyph->advance = (float)(g->advance.x >> 6);

    pen_x += gw + WR_GLYPH_PADDING;
    if (gh > row_height)
      row_height = gh;
  }

  glGenTextures(1, &font->texture);
  glBindTexture(GL_TEXTURE_2D, font->texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0,
               GL_RED, GL_UNSIGNED_BYTE, atlas_data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  font->atlas_width = atlas_width;
  font->atlas_height = atlas_height;

  free(atlas_data);
  return 0;
}

WrFont* wr_font_load(const char* path, float size)
{
  if (!g_ft_library) {
    if (wr_font_init() != 0)
      return NULL;
  }

  WrFont* font = calloc(1, sizeof(WrFont));
  if (!font)
    return NULL;

  if (FT_New_Face(g_ft_library, path, 0, &font->face) != 0) {
    fprintf(stderr, "Failed to load font: %s\n", path);
    free(font);
    return NULL;
  }

  font->size = size;
  FT_Set_Pixel_Sizes(font->face, 0, (FT_UInt)size);

  font->metrics.size = size;
  font->metrics.ascender = (float)(font->face->size->metrics.ascender >> 6);
  font->metrics.descender = (float)(font->face->size->metrics.descender >> 6);
  font->metrics.line_height = (float)(font->face->size->metrics.height >> 6);

  if (build_atlas(font) != 0) {
    FT_Done_Face(font->face);
    free(font);
    return NULL;
  }

  return font;
}

WrFont* wr_font_load_memory(const unsigned char* data, size_t data_size, float pixel_size)
{
  if (!g_ft_library) {
    if (wr_font_init() != 0)
      return NULL;
  }

  WrFont* font = calloc(1, sizeof(WrFont));
  if (!font)
    return NULL;

  if (FT_New_Memory_Face(g_ft_library, data, (FT_Long)data_size, 0, &font->face) != 0) {
    fprintf(stderr, "Failed to load font from memory\n");
    free(font);
    return NULL;
  }

  font->size = pixel_size;
  FT_Set_Pixel_Sizes(font->face, 0, (FT_UInt)pixel_size);

  font->metrics.size = pixel_size;
  font->metrics.ascender = (float)(font->face->size->metrics.ascender >> 6);
  font->metrics.descender = (float)(font->face->size->metrics.descender >> 6);
  font->metrics.line_height = (float)(font->face->size->metrics.height >> 6);

  if (build_atlas(font) != 0) {
    FT_Done_Face(font->face);
    free(font);
    return NULL;
  }

  return font;
}

void wr_font_destroy(WrFont* font)
{
  if (!font)
    return;

  if (font->texture)
    glDeleteTextures(1, &font->texture);

  if (font->face)
    FT_Done_Face(font->face);

  free(font);
}

WrFontMetrics wr_font_get_metrics(WrFont* font)
{
  WrFontMetrics empty = {0};
  if (!font)
    return empty;
  return font->metrics;
}

const WrGlyph* wr_font_get_glyph(WrFont* font, uint32_t codepoint)
{
  if (!font || codepoint >= 128)
    return NULL;

  WrGlyph* g = &font->glyphs[codepoint];
  if (g->codepoint == 0 && codepoint != 0)
    return NULL;

  return g;
}

uint32_t wr_font_get_texture(WrFont* font)
{
  if (!font)
    return 0;
  return font->texture;
}

void wr_font_get_atlas_size(WrFont* font, int* width, int* height)
{
  if (!font) {
    if (width) *width = 0;
    if (height) *height = 0;
    return;
  }
  if (width) *width = font->atlas_width;
  if (height) *height = font->atlas_height;
}

float wr_font_measure_text(WrFont* font, const char* text)
{
  if (!font || !text)
    return 0.0f;
  return wr_font_measure_text_n(font, text, strlen(text));
}

float wr_font_measure_text_n(WrFont* font, const char* text, size_t len)
{
  if (!font || !text || len == 0)
    return 0.0f;

  float width = 0.0f;
  for (size_t i = 0; i < len; i++) {
    unsigned char c = (unsigned char)text[i];
    if (c >= 128)
      continue;

    const WrGlyph* g = wr_font_get_glyph(font, c);
    if (g)
      width += g->advance;
  }

  return width;
}
