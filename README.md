# Walrus

<p align="center">
  <b>A modern C GUI toolkit for Linux, built for Wayland and X11.</b>
</p>

<p align="center">
  Lightweight • Native • Cross-backend • Written in C
</p>

---

## Overview

**Walrus** is a modern GUI toolkit written in pure C, designed to provide a simple and efficient way to build native Linux desktop applications.

Walrus provides an abstraction layer above low-level display protocols such as **Wayland** and **X11**, allowing developers to create applications without directly handling compositor communication, window management, input events, and rendering systems.

The goal of Walrus is to create a lightweight alternative to existing GUI frameworks while maintaining a simple C API, high performance, and native Linux integration.

---

## Features

### Core

- Native C API
- Application lifecycle management
- Event loop system
- Window management
- Input handling
- Object system
- Modular backend architecture

### Rendering

- Hardware accelerated rendering support
- OpenGL support
- Vulkan support (planned)
- Custom rendering pipeline

### Backends

- Wayland support
- X11 compatibility
- Extensible backend architecture

### UI Toolkit

- Window
- Button
- Label
- Text input
- Layout system
- Theme system
- Custom widgets

---

## Philosophy

Walrus follows several design principles:

### Simple API

Developers should be able to create applications without dealing with low-level system details.

Example:

```c
#include <walrus/walrus.h>

int main()
{
    wr_app *app = wr_app_create();

    wr_window *window =
        wr_window_create(
            "Hello Walrus",
            800,
            600
        );

    wr_app_run(app);

    return 0;
}
```

---

### Native First

Walrus does not try to emulate another operating system's design.

It is built specifically for Linux desktop environments with first-class support for:

- Wayland
- X11
- Linux graphics stack

---

### Lightweight Architecture

Walrus is designed as a library, not a complete desktop environment.

Application developers choose their own:

- Rendering backend
- Window behavior
- Application structure

---

# Architecture

```
Application
     |
     |
 Walrus API
     |
 +----------------+
 | Core           |
 | Event System   |
 | Widget System  |
 | Layout Engine  |
 +----------------+
     |
 +----------------+
 | Backend Layer  |
 +----------------+
     |
 +------------+
 | Wayland    |
 | X11        |
 +------------+
     |
 Linux System
```

---

# Project Structure

```
walrus/

├── include/
│   └── walrus/
│       ├── walrus.h
│       ├── window.h
│       └── widget.h
│
├── src/
│   ├── core/
│   ├── widgets/
│   ├── renderer/
│   └── backend/
│       ├── wayland/
│       └── x11/
│
├── examples/
│   └── hello.c
│
├── CMakeLists.txt
└── README.md
```

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
