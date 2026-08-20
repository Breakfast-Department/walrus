#include <stdio.h>
#include <stdlib.h>
#include <walrus/walrus.h>
#include <walrus/core/window.h>
#include <walrus/ui/widget.h>
#include <walrus/ui/style.h>
#include <walrus/text/font.h>
#include <walrus/text/text.h>
#include <walrus/renderer/batch.h>
#include <walrus/renderer/renderer.h>

static WrFont* g_font = NULL;
static WrFont* g_font_large = NULL;
static WrRenderSurface* g_surface = NULL;

static void text_widget_render(WrWidget* widget, WrRenderSurface* surface)
{
  (void)widget;
  if (!g_font || !g_font_large) return;

  WrRenderer* renderer = wr_get_renderer();
  if (!renderer || !renderer->draw_batch) return;

  WrBatch* batch = wr_batch_create();
  if (!batch) return;

  float x = widget->style.layout.margin.left + 20.0f;
  float y = widget->style.layout.margin.top + 20.0f;

  /* Draw a panel background */
  WrColor panel_bg = {0.18f, 0.18f, 0.22f, 1.0f};
  wr_batch_rounded_rect(batch, x, y, 350.0f, 200.0f, 12.0f, panel_bg);

  /* Draw heading */
  WrColor heading_color = {0.95f, 0.95f, 0.97f, 1.0f};
  wr_batch_text(batch, g_font_large, "Text Rendering Demo", x + 20.0f, y + 20.0f, heading_color);

  /* Draw body text */
  WrColor text_color = {0.75f, 0.75f, 0.80f, 1.0f};
  wr_batch_text(batch, g_font, "Walrus now supports font rendering!", x + 20.0f, y + 60.0f, text_color);
  wr_batch_text(batch, g_font, "Using FreeType for glyph rasterization.", x + 20.0f, y + 85.0f, text_color);
  wr_batch_text(batch, g_font, "GPU-accelerated with texture atlas.", x + 20.0f, y + 110.0f, text_color);

  /* Draw aligned text examples */
  WrColor accent_color = {0.4f, 0.7f, 0.9f, 1.0f};
  float center_x = x + 175.0f;

  wr_batch_text_aligned(batch, g_font, "Left aligned", x + 20.0f, y + 150.0f,
    accent_color, WR_TEXT_ALIGN_LEFT, WR_TEXT_BASELINE_TOP);

  wr_batch_text_aligned(batch, g_font, "Center", center_x, y + 150.0f,
    accent_color, WR_TEXT_ALIGN_CENTER, WR_TEXT_BASELINE_TOP);

  wr_batch_text_aligned(batch, g_font, "Right aligned", x + 330.0f, y + 150.0f,
    accent_color, WR_TEXT_ALIGN_RIGHT, WR_TEXT_BASELINE_TOP);

  /* Measure and display text width */
  const char* measure_text = "Measured text width";
  float text_width = wr_font_measure_text(g_font, measure_text);
  char width_str[64];
  snprintf(width_str, sizeof(width_str), "Width: %.1f px", text_width);

  wr_batch_text(batch, g_font, measure_text, x + 20.0f, y + 175.0f, text_color);
  WrColor dim_color = {0.5f, 0.5f, 0.55f, 1.0f};
  wr_batch_text(batch, g_font, width_str, x + 180.0f, y + 175.0f, dim_color);

  renderer->draw_batch(surface, batch);
  wr_batch_destroy(batch);
}

static WrFont* g_font = NULL;
static WrFont* g_font_large = NULL;
static WrRenderSurface* g_surface = NULL;

static void text_widget_render(WrWidget* widget, WrRenderSurface* surface)
{
  (void)widget;
  if (!g_font || !g_font_large) return;

  WrRenderer* renderer = wr_get_renderer();
  if (!renderer || !renderer->draw_batch) return;

  WrBatch* batch = wr_batch_create();
  if (!batch) return;

  float x = widget->style.layout.margin.left + 20.0f;
  float y = widget->style.layout.margin.top + 20.0f;

  /* Draw a panel background */
  WrColor panel_bg = {0.18f, 0.18f, 0.22f, 1.0f};
  wr_batch_rounded_rect(batch, x, y, 350.0f, 200.0f, 12.0f, panel_bg);

  /* Draw heading */
  WrColor heading_color = {0.95f, 0.95f, 0.97f, 1.0f};
  wr_batch_text(batch, g_font_large, "Text Rendering Demo", x + 20.0f, y + 20.0f, heading_color);

  /* Draw body text */
  WrColor text_color = {0.75f, 0.75f, 0.80f, 1.0f};
  wr_batch_text(batch, g_font, "Walrus now supports font rendering!", x + 20.0f, y + 60.0f, text_color);
  wr_batch_text(batch, g_font, "Using FreeType for glyph rasterization.", x + 20.0f, y + 85.0f, text_color);
  wr_batch_text(batch, g_font, "GPU-accelerated with texture atlas.", x + 20.0f, y + 110.0f, text_color);

  /* Draw aligned text examples */
  WrColor accent_color = {0.4f, 0.7f, 0.9f, 1.0f};
  float center_x = x + 175.0f;

  wr_batch_text_aligned(batch, g_font, "Left aligned", x + 20.0f, y + 150.0f,
    accent_color, WR_TEXT_ALIGN_LEFT, WR_TEXT_BASELINE_TOP);

  wr_batch_text_aligned(batch, g_font, "Center", center_x, y + 150.0f,
    accent_color, WR_TEXT_ALIGN_CENTER, WR_TEXT_BASELINE_TOP);

  wr_batch_text_aligned(batch, g_font, "Right aligned", x + 330.0f, y + 150.0f,
    accent_color, WR_TEXT_ALIGN_RIGHT, WR_TEXT_BASELINE_TOP);

  /* Measure and display text width */
  const char* measure_text = "Measured text width";
  float text_width = wr_font_measure_text(g_font, measure_text);
  char width_str[64];
  snprintf(width_str, sizeof(width_str), "Width: %.1f px", text_width);

  wr_batch_text(batch, g_font, measure_text, x + 20.0f, y + 175.0f, text_color);
  WrColor dim_color = {0.5f, 0.5f, 0.55f, 1.0f};
  wr_batch_text(batch, g_font, width_str, x + 180.0f, y + 175.0f, dim_color);

  renderer->draw_batch(surface, batch);
  wr_batch_destroy(batch);
}

int main(void)
{
  if (wr_init() != 0)
  {
    fprintf(stderr, "Failed initialize walrus\n");
    return EXIT_FAILURE;
  }

  WrWindow* window = wr_create_window("Walrus Text Demo", 800, 600);
  if (!window)
  {
    fprintf(stderr, "Failed create walrus window\n");
    wr_shutdown();
    return EXIT_FAILURE;
  }

  g_surface = wr_window_get_surface(window);
  if (!g_surface)
  {
    fprintf(stderr, "Failed get walrus render surface\n");
    wr_window_destroy(window);
    wr_shutdown();
    return EXIT_FAILURE;
  }

  /* Load fonts */
  const char* font_paths[] = {
    "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/noto/NotoSans-Regular.ttf",
    NULL
  };

  for (int i = 0; font_paths[i] && !g_font; i++) {
    g_font = wr_font_load(font_paths[i], 14.0f);
    if (g_font) {
      g_font_large = wr_font_load(font_paths[i], 20.0f);
    }
  }

  if (!g_font) {
    fprintf(stderr, "Warning: Could not load any font\n");
  }

  /* Create a text demo widget using custom render callback */
  WrWidget* text_widget = wr_create_widget("text-demo");
  text_widget->render = text_widget_render;

  WrElement text_el = { .type = WR_ELEMENT_TYPE_WIDGET, .data = text_widget };
  wr_window_add_child(window, &text_el);

  /* Main loop */
  while (wr_window_should_close(window) == 0)
  {
    
  /* Create a widget in the content area */
  WrWidget* content_widget = wr_create_widget("content-panel");
  content_widget->style.background = (WrBackground){
    .type = WR_BACKGROUND_COLOR,
    .color = { 0.3f, 0.5f, 0.7f, 1.0f }
  };

  for (int i = 0; font_paths[i] && !g_font; i++) {
    g_font = wr_font_load(font_paths[i], 14.0f);
    if (g_font) {
      g_font_large = wr_font_load(font_paths[i], 20.0f);
    }
  }

  if (!g_font) {
    fprintf(stderr, "Warning: Could not load any font\n");
  }

  /* Create a text demo widget using custom render callback */
  WrWidget* text_widget = wr_create_widget("text-demo");
  text_widget->render = text_widget_render;

  WrElement text_el = { .type = WR_ELEMENT_TYPE_WIDGET, .data = text_widget };
  wr_window_add_child(window, &text_el);

  /* Main loop */
  while (wr_window_should_close(window) == 0)
  {
    if (g_interrupt_requested) { wr_window_set_should_close(window, 1); break; }

    wr_poll_events();
    wr_render();
  }

  /* Cleanup */
  if (g_font_large) wr_font_destroy(g_font_large);
  if (g_font) wr_font_destroy(g_font);

  wr_window_destroy(window);
  wr_shutdown();
  return EXIT_SUCCESS;
}
