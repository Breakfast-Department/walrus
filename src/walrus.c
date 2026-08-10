#include <walrus/walrus.h>

int wr_init(void)
{
  WrBackend* backend = wr_backend_init();
  if (!backend)
    return -1;

  if (!backend->init())
    return -1;

  return 0;
}
