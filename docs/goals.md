# Walrus Goals

This document defines the development goals and release requirements for Walrus.

It is used to track implemented features, incomplete systems, and features required before each release.

---

# Current Status

Walrus is currently in the **architecture and foundation stage**.

The main abstraction layers have been established, but several systems are still incomplete and contain only initial declarations or skeleton implementations.

Current architecture:

```text
Walrus
│
├── Core
│   ├── Window
│   ├── Event
│   └── Input
│
├── Backend
│   ├── Wayland
│   └── X11
│
└── Renderer
    ├── OpenGL
    │   ├── EGL
    │   ├── Shader
    │   ├── Vertex
    │   └── Batch
    │
    └── Renderer Abstraction
```

---

# Status Legend

* [X] Implemented
* [ ] Partially implemented
* [ ] Not implemented

---

# Core

## Window System

* [X] `WrWindow` abstraction
* [X] Window title
* [X] Window width and height
* [X] Backend-specific window data
* [X] Window creation abstraction
* [X] Window destruction abstraction
* [ ] Window lifecycle management
* [ ] Window resize handling
* [ ] Window position
* [ ] Window state management
* [ ] Fullscreen
* [ ] Maximized state
* [ ] Minimized state
* [ ] Window focus state
* [ ] Multiple window management

---

# Event System

* [ ] `WrEvent` foundation
* [ ] Event abstraction
* [ ] Event queue
* [ ] Event polling API
* [ ] Event dispatch
* [ ] Window events
* [ ] Keyboard events
* [ ] Mouse events
* [ ] Touch events
* [ ] Text input events
* [ ] Event propagation

---

# Input System

* [ ] Input abstraction
* [ ] Keyboard state
* [ ] Mouse state
* [ ] Mouse position
* [ ] Mouse buttons
* [ ] Mouse scrolling
* [ ] Keyboard modifiers
* [ ] Key repeat
* [ ] Text input
* [ ] Touch input
* [ ] Gamepad input
* [ ] Controller support

The input system should remain independent from the UI system so that it can also be used by applications and game engines.

---

# Backend

## Backend Abstraction

* [X] `WrBackend`
* [X] Backend function-pointer interface
* [X] Backend initialization
* [X] Backend shutdown
* [X] Event polling interface
* [X] Window creation interface
* [X] Window destruction interface
* [ ] Backend selection
* [ ] Runtime backend selection API
* [ ] Backend capability detection

---

# Wayland

* [X] Wayland display connection
* [X] Registry discovery
* [X] `wl_compositor`
* [X] `xdg_wm_base`
* [X] `wl_surface`
* [X] `xdg_surface`
* [X] `xdg_toplevel`
* [X] Window title
* [X] Initial surface commit
* [ ] Window lifecycle
* [ ] Configure event foundation
* [ ] Proper configure/resize handling
* [ ] `xdg_toplevel.configure`
* [ ] Window close event
* [ ] Frame callbacks
* [ ] `wl_seat`
* [ ] Pointer input
* [ ] Keyboard input
* [ ] Touch input
* [ ] Text input
* [ ] Output handling
* [ ] HiDPI / fractional scaling
* [ ] Decorations
* [ ] Clipboard
* [ ] Drag and drop

---

# X11

The X11 backend currently exists primarily as an architectural placeholder.

* [X] X11 backend structure
* [ ] X11 display connection
* [ ] X11 window creation
* [ ] X11 window destruction
* [ ] X11 event handling
* [ ] X11 keyboard input
* [ ] X11 mouse input
* [ ] X11 resize handling
* [ ] X11 window states
* [ ] X11 renderer integration

---

# Renderer

## Renderer Abstraction

* [X] `WrRenderer`
* [X] Renderer function-pointer architecture
* [ ] Renderer initialization
* [ ] Renderer shutdown
* [ ] Render surface abstraction
* [ ] Frame lifecycle
* [ ] Begin frame
* [ ] End frame
* [ ] Present
* [ ] Renderer resource management
* [ ] Renderer selection
* [ ] Multi-window rendering

The renderer abstraction must not expose OpenGL, EGL, Vulkan, or other graphics API types to the core layer.

---

# OpenGL

The OpenGL renderer currently exists as a **foundation/skeleton**.

## OpenGL API

* [X] `opengl.h`
* [ ] OpenGL renderer structure
* [ ] Function declarations
* [ ] Function definitions
* [ ] OpenGL context initialization
* [ ] OpenGL function loading
* [ ] OpenGL state initialization
* [ ] Framebuffer management
* [ ] Clear
* [ ] Draw calls
* [ ] Buffer management
* [ ] Texture management
* [ ] Shader management
* [ ] Render pipeline
* [ ] GPU resource cleanup

`opengl.h` and `opengl.c` currently contain the initial API structure, but the functions are not fully implemented.

---

# EGL

EGL currently exists as a **minimal abstraction layer**.

* [X] `egl.h`
* [X] `egl.c`
* [X] EGL API structure
* [ ] EGL abstraction
* [ ] EGL display initialization
* [ ] EGL configuration selection
* [ ] EGL context creation
* [ ] EGL surface creation
* [ ] `eglMakeCurrent`
* [ ] Swap buffers
* [ ] EGL cleanup
* [ ] Wayland EGL integration
* [ ] X11 EGL integration
* [ ] Multi-window EGL surface management

The current EGL implementation is intentionally minimal and serves as the foundation for the OpenGL renderer.

---

# GLAD

* [X] GLAD integration
* [X] OpenGL loader source
* [X] EGL loader source
* [X] GLAD headers
* [ ] Loader initialization
* [ ] Renderer initialization integration

GLAD is responsible only for loading graphics API functions.

It does not manage windows, surfaces, or rendering resources.

---

# Vertex System

* [X] `vertex.h`
* [ ] Vertex abstraction
* [ ] Vertex buffer implementation
* [ ] Vertex attribute configuration
* [ ] GPU upload
* [ ] Dynamic vertex buffers
* [ ] Buffer resizing

---

# Batch System

* [X] `batch.h`
* [ ] `WrBatch` foundation
* [ ] Dynamic vertex storage
* [ ] Dynamic index storage
* [ ] Texture grouping
* [ ] Draw command grouping
* [ ] Batch submission
* [ ] Batch flushing
* [ ] Batch optimization

The batch system is intended to combine geometry from multiple UI objects into fewer GPU draw calls.

---

# Shader System

* [ ] `shader.h`
* [ ] Shader abstraction
* [ ] Shader creation
* [ ] Shader compilation
* [ ] Shader linking
* [ ] Uniform management
* [ ] Shader destruction
* [ ] Default UI shaders
* [ ] Shader error reporting

---

# UI Rendering

The UI renderer has not been implemented yet.

* [ ] Rectangle rendering
* [ ] Rounded rectangle rendering
* [ ] Border rendering
* [ ] Image rendering
* [ ] Text rendering
* [ ] Clipping
* [ ] Scissor rectangles
* [ ] Transformations
* [ ] Opacity
* [ ] Layering
* [ ] Z-order
* [ ] UI batching

---

# Text Rendering

* [ ] Font loading
* [ ] Font management
* [ ] Glyph rasterization
* [ ] Glyph atlas
* [ ] Text shaping
* [ ] Unicode support
* [ ] Text measurement
* [ ] Font fallback
* [ ] GPU text rendering

---

# UI System

The UI layer is planned to sit above the core and renderer.

* [ ] Widget system
* [ ] Widget tree
* [ ] Layout system
* [ ] Styling system
* [ ] Components
* [ ] Hit testing
* [ ] Focus management
* [ ] Hover state
* [ ] Active state
* [ ] Keyboard navigation
* [ ] Accessibility

---

# Documentation

* [X] `architecture.md`
* [X] `rendering.md`
* [X] `wayland.md`
* [X] `input.md`
* [X] `goals.md`
* [ ] API documentation
* [ ] Developer documentation
* [ ] Renderer implementation guide
* [ ] Backend implementation guide
* [ ] Example documentation

---

# Build System

* [X] CMake project
* [X] Automatic source discovery
* [X] GLAD integration
* [X] Shared library build
* [X] Example application
* [ ] Installation rules
* [ ] Public header installation
* [ ] `pkg-config`
* [ ] CMake package support
* [ ] Dependency detection
* [ ] Debug build configuration
* [ ] Release build configuration
* [ ] CI build

---

# Testing

Testing infrastructure has not been implemented yet.

* [ ] Core unit tests
* [ ] Window tests
* [ ] Backend tests
* [ ] Event tests
* [ ] Input tests
* [ ] Renderer tests
* [ ] OpenGL tests
* [ ] EGL tests
* [ ] Multi-window tests
* [ ] Integration tests
* [ ] Example application tests

---

# Release Goals

## v0.1 — Foundation

The goal of `v0.1` is to establish a functional platform and rendering foundation.

### Required

* [X] Core window abstraction
* [X] Backend abstraction
* [X] Wayland connection
* [X] Wayland window creation
* [ ] Event system
* [ ] Renderer abstraction
* [ ] EGL abstraction
* [ ] OpenGL renderer structure
* [X] GLAD integration
* [X] CMake build
* [X] Basic example

### Not required

The following do not need to be complete for `v0.1`:

* Vulkan
* X11
* Full UI system
* Text rendering
* Gamepad
* Advanced input
* Production-quality batching

---

# v0.2 — Functional Renderer

The goal of `v0.2` is to make the OpenGL renderer actually render graphics.

### Required

* [ ] Working EGL initialization
* [ ] Working OpenGL context
* [ ] GLAD initialization
* [ ] Working render surface
* [ ] Begin/end frame
* [ ] Clear screen
* [ ] Vertex buffer
* [ ] Index buffer
* [ ] Basic shader system
* [ ] Basic draw call
* [ ] Working `WrBatch`
* [ ] Render a basic rectangle

Target:

```text
Walrus
  │
  ▼
Wayland
  │
  ▼
EGL
  │
  ▼
OpenGL
  │
  ▼
GPU
  │
  ▼
Rendered Rectangle
```

---

# v0.3 — UI Rendering

The goal of `v0.3` is to render the first real UI primitives.

### Required

* [ ] Rectangle
* [ ] Rounded rectangle
* [ ] Border
* [ ] Color
* [ ] Opacity
* [ ] Clipping
* [ ] Batching
* [ ] Basic UI renderer
* [ ] Basic widget rendering

Target:

```text
┌─────────────────────────────┐
│                             │
│       Walrus UI             │
│                             │
│    ┌─────────────────┐      │
│    │     Button      │      │
│    └─────────────────┘      │
│                             │
└─────────────────────────────┘
```

---

# v0.4 — Input and Interaction

The goal of `v0.4` is to make the UI interactive.

### Required

* [ ] Event queue
* [ ] Keyboard input
* [ ] Mouse input
* [ ] Mouse position
* [ ] Mouse buttons
* [ ] Hover state
* [ ] Click handling
* [ ] Focus handling
* [ ] Keyboard navigation
* [ ] Window close events
* [ ] Window resize events

---

# v0.5 — Text and Components

The goal of `v0.5` is to provide practical UI components.

### Required

* [ ] Font loading
* [ ] Text rendering
* [ ] Glyph atlas
* [ ] Text measurement
* [ ] Basic layout
* [ ] Button
* [ ] Label
* [ ] Image
* [ ] Input field
* [ ] Panel/container

---

# v0.6 — Multi-Backend

The goal of `v0.6` is to validate the abstraction architecture.

### Required

* [ ] Complete X11 backend
* [ ] X11 window creation
* [ ] X11 event handling
* [ ] X11 renderer integration
* [ ] Backend selection
* [ ] Multi-window support
* [ ] Renderer-independent core

The same application should be able to run without changing its core code:

```text
Application
    │
    ▼
Walrus
    │
    ├── Wayland + OpenGL
    │
    └── X11 + OpenGL
```

---

# v0.7 — Advanced Rendering

* [ ] Improved batching
* [ ] Texture atlas
* [ ] Advanced clipping
* [ ] Transform system
* [ ] Render targets
* [ ] Offscreen rendering
* [ ] GPU resource management
* [ ] Performance profiling
* [ ] Renderer optimization

---

# v0.8 — Vulkan Renderer

Vulkan should only be implemented after the renderer abstraction has proven sufficient with OpenGL.

### Required

* [ ] Vulkan instance
* [ ] Physical device selection
* [ ] Logical device
* [ ] Queue management
* [ ] `VkSurfaceKHR`
* [ ] Swapchain
* [ ] Command buffers
* [ ] Synchronization
* [ ] Pipeline
* [ ] Shader system
* [ ] Buffer management
* [ ] Texture management
* [ ] Renderer integration

Target architecture:

```text
WrRenderer
    │
    ├── OpenGL
    │
    └── Vulkan
```

---

# v1.0 — Stable Walrus

`v1.0` represents a stable public API suitable for external applications.

### Required

* [ ] Stable core API
* [ ] Stable backend API
* [ ] Stable renderer API
* [ ] Wayland backend
* [ ] X11 backend
* [ ] OpenGL renderer
* [ ] Vulkan renderer
* [ ] Multi-window support
* [ ] Input system
* [ ] UI system
* [ ] Text rendering
* [ ] Documentation
* [ ] API reference
* [ ] Tests
* [ ] CI
* [ ] Installation support
* [ ] Semantic versioning

---

# Current Priority

Development should currently focus on the following order:

```text
1. Wayland window lifecycle
          │
          ▼
2. Render surface abstraction
          │
          ▼
3. EGL implementation
          │
          ▼
4. OpenGL context initialization
          │
          ▼
5. GLAD initialization
          │
          ▼
6. Basic OpenGL rendering
          │
          ▼
7. Vertex + Index buffers
          │
          ▼
8. Shader system
          │
          ▼
9. WrBatch
          │
          ▼
10. Basic UI rendering
```

Do not implement Vulkan before the renderer abstraction has been validated through a working OpenGL renderer.

---

# Definition of Done

A feature is considered complete when:

* Its public API is defined.
* Its implementation works on the supported platform.
* Resources are correctly created and destroyed.
* Failure cases are handled.
* It does not unnecessarily expose platform-specific implementation details.
* It has at least a basic usage example when appropriate.
* Its architectural role is documented.

---

# Architecture Goal

The long-term goal of Walrus is to maintain a strict separation:

```text
┌───────────────────────────────┐
│           Application         │
├───────────────────────────────┤
│              UI               │
├───────────────────────────────┤
│             Core              │
├───────────────┬───────────────┤
│    Backend    │    Renderer   │
├───────────────┼───────────────┤
│ Wayland / X11 │ OpenGL/Vulkan │
└───────────────┴───────────────┘
```

The core should never need to know how a specific backend or graphics API works.

The renderer should only receive rendering data and decide how to submit it to the GPU.

The backend should only manage the native platform.

This separation is the primary architectural goal of Walrus.
