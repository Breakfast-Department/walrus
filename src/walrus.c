#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <stdio.h>
#include <signal.h>

#include <walrus/walrus.h>
#include "walrus/core/window.h"
#include "walrus/text/font.h"
#include "walrus/ui/widget.h"

static volatile sig_atomic_t g_walrus_sigint = 0;

static void walrus_sigint_handler(int sig)
{
  (void)sig;
  g_walrus_sigint = 1;
}

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

  {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = walrus_sigint_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
  }

  return 0;
}

void wr_poll_events(void)
{
  WrBackend* backend = wr_get_backend();
  if (backend && backend->poll_events) {
    sigset_t set, oldset;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, &oldset);
    backend->poll_events();
    sigprocmask(SIG_SETMASK, &oldset, NULL);
  }

  if (g_walrus_sigint) {
    extern void wr_request_close_all(void);
    wr_request_close_all();
    g_walrus_sigint = 0;
  }
}

void wr_shutdown(void)
{
  wr_font_shutdown();

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
