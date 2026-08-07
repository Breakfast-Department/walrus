// hello.c

#include "walrus/walrus.h"
#include "stdio.h"

int main() {
  // Initialize the Walrus runtime
  wr_init();

  // create window
  wr_window_t *window = wr_create_window("Hello, Walrus!", 800, 600);

  while(wr_should_close(window) == 0) {
    // Poll events
    wr_poll_events();
  }

  wr_window_destroy(window);
  wr_shutdown();

  return 0;
}
