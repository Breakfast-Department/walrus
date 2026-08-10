# Walrus

Walrus is a modular C GUI toolkit for Linux. It provides a simple application API while isolating backend and renderer details from user code.

## Overview

Walrus focuses on native Linux desktop applications. It hides platform initialization and renderer setup so developers can write clean application logic without direct interaction with Wayland, X11, or OpenGL.

## What Walrus Offers

- Simple, stable C API for window management and rendering
- Backend abstraction for Wayland and X11
- Renderer abstraction for GPU-backed presentation
- A foundation for future renderer implementations

## Design Goals

- Keep the public interface concise and easy to use
- Avoid leaking platform-specific details into application code
- Separate backend and renderer implementations
- Support growth without breaking the application interface

## Supported Platforms

- Wayland
- X11

## Requirements

- Linux
- C compiler
- CMake 3.20 or newer
- pkg-config
- Wayland development libraries for Wayland support
- X11 development libraries for X11 support

## Build

```bash
git clone https://github.com/example/walrus.git
cd walrus
cmake -B build
cmake --build build
```

Install with:

```bash
sudo cmake --install build
```

## Quick Start

A minimal Walrus application uses only the public API:

```c
#include <stdio.h>
#include <walrus/walrus.h>
#include <walrus/core/window.h>

int main(void)
{
    if (wr_init() != 0)
    {
        fprintf(stderr, "Failed to initialize Walrus\n");
        return 1;
    }

    WrWindow *window = wr_create_window("Hello, Walrus", 800, 600);
    if (!window)
    {
        wr_shutdown();
        return 1;
    }

    WrRenderSurface *surface = wr_window_get_surface(window);

    for (int frame = 0; frame < 300; ++frame)
    {
        wr_poll_events();
        wr_begin_frame(surface);
        wr_clear(frame / 300.0f, 0.3f, 1.0f - frame / 300.0f, 1.0f);
        wr_end_frame(surface);
    }

    wr_window_destroy(window);
    wr_shutdown();
    return 0;
}
```

## Documentation

See the `docs` directory for architecture and design details.

## Status

Walrus is in early development. The toolkit is designed for flexibility and gradual expansion.

---

# Requirements

## Linux

Supported platforms:

- Linux distributions
- Wayland compositor
- X11 display server

Dependencies:

- C compiler
- CMake >= 3.20
- pkg-config
- Wayland development libraries
- X11 development libraries

Example (Arch Linux):

```bash
sudo pacman -S cmake gcc pkgconf wayland libx11
```

Example (Debian/Ubuntu):

```bash
sudo apt install \
cmake \
gcc \
pkg-config \
libwayland-dev \
libx11-dev
```

---

# Building

Clone repository:

```bash
git clone https://github.com/example/walrus.git

cd walrus
```

Create build directory:

```bash
cmake -B build
```

Build:

```bash
cmake --build build
```

Install:

```bash
sudo cmake --install build
```

---

# Example

Create a simple application:

```c
#include <walrus/walrus.h>

int main()
{
    wr_app *app = wr_app_create();

    wr_window *window =
        wr_window_create(
            "Walrus Example",
            800,
            600
        );

    wr_window_show(window);

    wr_app_run(app);

    return 0;
}
```

Compile:

```bash
gcc example.c \
    -lwalrus \
    -o example
```

Run:

```bash
./example
```

---

# Development Roadmap

## Phase 1 — Core

- [ ] Application system
- [ ] Event loop
- [ ] Window abstraction
- [ ] Wayland backend
- [ ] X11 backend

## Phase 2 — Rendering

- [ ] Software renderer
- [ ] OpenGL renderer
- [ ] Vulkan renderer

## Phase 3 — Widgets

- [ ] Button
- [ ] Label
- [ ] Text input
- [ ] Containers
- [ ] Layout engine

## Phase 4 — Advanced

- [ ] Theme engine
- [ ] Animation system
- [ ] Accessibility support
- [ ] Internationalization

---

# Contributing

Contributions are welcome.

Before contributing:

1. Fork the repository.
2. Create a feature branch.

```bash
git checkout -b feature/new-widget
```

3. Commit changes.

```bash
git commit -m "Add new widget"
```

4. Push branch.

```bash
git push origin feature/new-widget
```

5. Open a Pull Request.

---

# License

Walrus is licensed under the MIT License.

See [LICENSE](LICENSE) for details.

---

# Status

Walrus is currently in early development.

The API may change while the core architecture is being designed.

---

<p align="center">
  Built with C and Linux ❤️
</p>
