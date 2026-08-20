#ifndef WALRUS_H
#define WALRUS_H

#include <walrus/backend/backend.h>
#include <walrus/renderer/renderer.h>

int wr_init(void);
void wr_poll_events(void);
void wr_shutdown(void);

int wr_begin_frame(WrRenderSurface *surface);
int wr_end_frame(WrRenderSurface *surface);
void wr_clear(float r, float g, float b, float a);

void wr_render(void);

#endif
