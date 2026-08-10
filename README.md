# Walrus

**Walrus** is a modular C GUI toolkit for Linux with a simple application API and a backend-independent rendering architecture.

Walrus is designed to hide platform-specific details such as Wayland, X11, EGL, and OpenGL behind a small and stable public API.

> **Status:** Early development — API and architecture may change.

## Features

* Native Linux GUI toolkit written in C
* Wayland and X11 backend support
* Backend abstraction
* Renderer abstraction
* GPU-backed rendering
* Small public API
* Designed for extensibility

## Architecture

Walrus separates application code from platform and rendering implementations:

```text
Application
    │
    ▼
Walrus API
    │
    ├── Window
    ├── Event Loop
    └── Rendering
         │
         ├── OpenGL
         ├── Vulkan
         └── Software
    │
    ▼
Backend
    ├── Wayland
    └── X11
```

The application only interacts with the public Walrus API. Backend and renderer implementations are handled internally by the toolkit.

For more details, see [`docs/architecture.md`](docs/architecture.md).

## Requirements

* Linux
* C compiler
* CMake 3.20+
* pkg-config
* Wayland development libraries
* X11 development libraries

### Arch Linux

```bash
sudo pacman -S cmake gcc pkgconf wayland libx11
```

### Debian / Ubuntu

```bash
sudo apt install \
    cmake \
    gcc \
    pkg-config \
    libwayland-dev \
    libx11-dev
```

## Building

Clone the repository:

```bash
git clone https://github.com/ezravln/walrus.git
cd walrus
```

Configure and build:

```bash
cmake -B build
cmake --build build
```

Install:

```bash
sudo cmake --install build
```

## Example

A minimal Walrus application:

```c
#include <stdio.h>
#include <stdlib.h>

#include <walrus/walrus.h>
#include <walrus/core/window.h>

int main(void)
{
    if (wr_init() != 0)
    {
        fprintf(stderr, "Failed to initialize Walrus\n");
        return EXIT_FAILURE;
    }

    WrWindow *window =
        wr_create_window("Hello, Walrus", 800, 600);

    if (!window)
    {
        wr_shutdown();
        return EXIT_FAILURE;
    }

    WrRenderSurface *surface =
        wr_window_get_surface(window);

    while (!wr_window_should_close(window))
    {
        wr_poll_events();

        wr_begin_frame(surface);

        wr_clear(
            0.1f,
            0.1f,
            0.1f,
            1.0f
        );

        wr_end_frame(surface);
    }

    wr_window_destroy(window);
    wr_shutdown();

    return EXIT_SUCCESS;
}
```

See [`examples/hello.c`](examples/hello.c) for a complete example.

## Contributing

Contributions are welcome.

Before opening a pull request, please read [`CONTRIBUTING.md`](CONTRIBUTING.md).

Typical workflow:

```bash
git checkout -b feature/my-feature
git commit -m "Add my feature"
git push origin feature/my-feature
```

Then open a Pull Request.

## License

Walrus is licensed under the MIT License.

See [`LICENSE`](LICENSE) for details.

---

<p align="center">
  Built with C and Linux ❤️
</p>
