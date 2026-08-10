#include <stdio.h>
#include <walrus/walrus.h>
#include <walrus/core/window.h>


int main() {
  if (!wr_init())
  {
    fprintf(stderr, "Failed initialized walrus\n");
  }

  WrWindow* window = wr_create_window("Hello, Walrus!", 800, 600);

  return 0;
}
