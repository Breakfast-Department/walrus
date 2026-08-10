#ifndef WR_RENDERER_H
#define WR_RENDERER_H

#include <walrus/core/window.h>

typedef struct WrBatch WrBatch;

typedef struct WrRenderSurface {
  void *native_window;
  void *renderer_data;
} WrRenderSurface;

typedef struct WrRenderer {
  int  (*init)(void *native_display);
  void (*shutdown)(void);

  WrRenderSurface *(*create_surface)(void *native_window);
  void (*destroy_surface)(WrRenderSurface *surface);

  int  (*begin_frame)(WrRenderSurface *surface);
  int  (*end_frame)(WrRenderSurface *surface);

  void (*clear)(float r, float g, float b, float a);

  void (*draw_batch)(
    WrRenderSurface *surface,
    WrBatch *batch
  );
} WrRenderer;

WrRenderer* wr_renderer_init(void);
WrRenderer* wr_get_renderer(void);

#endif
