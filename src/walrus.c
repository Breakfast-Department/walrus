#include <stdio.h>
#include <walrus/walrus.h>

int wr_init(void)
{
  WrBackend* backend = wr_backend_init();
  if (!backend)
  {
    fprintf(stderr, "walrus: backend selection failed\n");
    return -1;
  }

  if (backend->init() != 0)
  {
    fprintf(stderr, "walrus: backend initialization failed\n");
    return -1;
  }

  WrRenderer* renderer = wr_renderer_init();
  if (!renderer)
  {
    fprintf(stderr, "walrus: renderer selection failed\n");
    backend->shutdown();
    return -1;
  }

  if (renderer->init(backend->get_native_display()) != 0)
  {
    fprintf(stderr, "walrus: renderer initialization failed\n");
    backend->shutdown();
    return -1;
  }

  return 0;
}

void wr_poll_events(void)
{
  WrBackend* backend = wr_get_backend();
  if (backend && backend->poll_events)
    backend->poll_events();
}

void wr_shutdown(void)
{
  WrRenderer* renderer = wr_get_renderer();
  if (renderer && renderer->shutdown)
    renderer->shutdown();

  WrBackend* backend = wr_get_backend();
  if (backend && backend->shutdown)
    backend->shutdown();
}

int wr_begin_frame(WrRenderSurface *surface)
{
  WrRenderer* renderer = wr_get_renderer();
  if (!renderer || !renderer->begin_frame)
    return -1;
  return renderer->begin_frame(surface);
}

int wr_end_frame(WrRenderSurface *surface)
{
  WrRenderer* renderer = wr_get_renderer();
  if (!renderer || !renderer->end_frame)
    return -1;
  return renderer->end_frame(surface);
}

void wr_clear(float r, float g, float b, float a)
{
  WrRenderer* renderer = wr_get_renderer();
  if (!renderer || !renderer->clear)
    return;
  renderer->clear(r, g, b, a);
}
