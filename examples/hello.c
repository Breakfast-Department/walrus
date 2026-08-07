// hello.c

#include "walrus/walrus.h"
#include "stdio.h"

int main() {
  // Initialize the Walrus runtime
  wr_init();

  // Detect the active display backend
  if (wr_get_backend()->type == WR_BACKEND_WAYLAND) {
    printf("Running on Wayland\n");
  } else {
    printf("Running on X11\n");
  }

  printf("Hello, World!");
  return 0;
}
