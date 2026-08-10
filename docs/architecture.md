# Walrus Architecture

Walrus is built as a modular toolkit for Linux. It keeps the application API separate from platform backends and renderer implementations.

## Design Overview

Walrus has three main layers:

- **Public API**: the interface used by applications
- **Backend**: platform-specific window and event handling
- **Renderer**: graphics output and presentation

Each layer is designed to be independent so that the toolkit can support multiple backends and renderer implementations without changing application code.

## Public API

The public API is the only layer application code should use directly. It provides:

- initialization and shutdown
- window creation and destruction
- event polling
- frame lifecycle and clearing

This layer should remain stable even as backend or renderer implementations evolve.

## Core Concepts

The core contains platform-independent abstractions such as windows, event polling, and rendering surfaces. It does not expose platform-specific or graphics-API-specific types.

## Backend Layer

Backends implement the platform-specific behavior for native windows and event delivery. Current backends include:

- Wayland
- X11

The backend layer is responsible for:

- connecting to the display server
- creating and destroying native windows
- polling platform events

## Renderer Layer

The renderer layer handles frame setup, content drawing, and buffer presentation. Current implementation work focuses on OpenGL.

The renderer interface is intentionally generic so additional backends such as Vulkan or custom renderers can be added later.

## Render Surfaces

A render surface connects a window to a renderer. This allows the toolkit to manage presentation without exposing native graphics handles in application code.

## Goals

Walrus is intended to provide a practical foundation for Linux GUI applications with:

- a simple C API
- a modular backend strategy
- a renderer abstraction that can grow over time

For implementation details and design rationale, see the rest of the documentation.

    void *native_data;
    void *renderer_data;
} WrRenderSurface;
```

The actual implementation is owned by the renderer.

For OpenGL:

```text
WrRenderSurface
       │
       └── renderer_data
              │
              └── WrOpenGLSurface
                       │
                       └── EGLSurface
```

This allows the same `WrWindow` abstraction to work with multiple rendering APIs.

---

# 6. OpenGL Renderer

The OpenGL implementation lives under:

```text
renderer/opengl/
```

Its responsibilities include:

* EGL initialization
* OpenGL context management
* OpenGL surface management
* shader management
* vertex buffers
* index buffers
* batching
* drawing

The layers are:

```text
WrRenderer
    │
    ▼
OpenGL Renderer
    │
    ├── EGL
    │
    ├── GLAD
    │
    ├── Shader
    │
    ├── Vertex
    │
    └── Batch
```

### EGL

EGL connects the native window system to OpenGL.

```text
Wayland/X11
     │
     ▼
    EGL
     │
     ▼
OpenGL Context
```

EGL is therefore an implementation detail of the OpenGL renderer.

`WrRenderer` should not expose EGL functions directly.

### GLAD

GLAD loads OpenGL functions.

For example:

```c
glCreateShader();
glBindBuffer();
glDrawElements();
```

GLAD is a dependency of the OpenGL renderer, not part of the Walrus public API.

---

# 7. Rendering Data

Rendering data should also remain separated from the window and backend systems.

## Vertex

`vertex.h` contains the vertex representation used by the renderer.

```text
renderer/opengl/vertex.h
```

Example:

```c
typedef struct WrVertex {
    float x;
    float y;

    float r;
    float g;
    float b;
    float a;

    float u;
    float v;
} WrVertex;
```

The exact layout may evolve as the renderer becomes more advanced.

---

## Batch

`batch.h` contains the representation of geometry submitted to the renderer.

```text
renderer/opengl/batch.h
```

Conceptually:

```text
UI Objects
   │
   ├── Button
   ├── Text
   ├── Image
   └── Shape
        │
        ▼
     WrBatch
        │
        ├── vertices
        └── indices
                │
                ▼
              GPU
```

Batching allows multiple objects to be rendered together instead of issuing a separate draw call for every object.

---

# 8. Window Creation Flow

Creating a window should follow this general flow:

```text
wr_create_window()
        │
        ▼
    WrWindow
        │
        ▼
    WrBackend
        │
        ├── Wayland
        │     └── wl_surface
        │
        └── X11
              └── X11 Window
        │
        ▼
   WrRenderSurface
        │
        ▼
    WrRenderer
        │
        ├── OpenGL
        ├── Vulkan
        └── Custom
```

The window itself should not directly create an EGL context or Vulkan context.

---

# 9. Rendering Flow

A frame should conceptually look like:

```text
Application
    │
    ▼
Update UI / Game State
    │
    ▼
Generate Render Data
    │
    ▼
WrBatch
    │
    ▼
WrRenderer
    │
    ▼
Begin Frame
    │
    ▼
Draw Batch
    │
    ▼
End Frame
```

For OpenGL:

```text
Begin Frame
    │
    ▼
eglMakeCurrent()
    │
    ▼
OpenGL rendering
    │
    ▼
eglSwapBuffers()
```

For Vulkan, the same high-level renderer interface can map to:

```text
Begin Frame
    │
    ▼
Acquire Swapchain Image
    │
    ▼
Record Command Buffer
    │
    ▼
Submit
    │
    ▼
Present
```

The application does not need to know these differences.

---

# 10. Multi-Window

Walrus should use a shared renderer where possible.

Conceptually:

```text
                  WrRenderer
                      │
             Shared renderer state
                      │
        ┌─────────────┼─────────────┐
        │             │             │
     Window A      Window B      Window C
        │             │             │
   Surface A      Surface B      Surface C
```

For OpenGL, the renderer can share the EGL display/context while each window has its own rendering surface.

The exact resource-sharing strategy belongs to the renderer implementation.

---

# 11. Dependency Rules

The following dependency direction should be maintained:

```text
Public API
    ↓
Core
    ↓
Backend / Renderer
    ↓
Platform / Graphics APIs
```

Higher-level layers should not depend on implementation details from lower-level layers.

For example:

```text
GOOD:

WrWindow
   ↓
WrRenderSurface
   ↓
OpenGL implementation
   ↓
EGL
```

Avoid:

```text
BAD:

WrWindow
   ↓
EGLSurface
```

because this would make the core window system dependent on OpenGL.

---

# 12. Design Principles

Walrus follows several architectural principles.

### Platform Independence

The core API should not depend on Wayland or X11.

### Renderer Independence

The UI and application layers should not depend on OpenGL, Vulkan, or another graphics API.

### Hidden Implementation Details

Platform and renderer-specific structures should remain private whenever possible.

### Shared Interfaces

Wayland and X11 should implement the same backend abstraction.

OpenGL and Vulkan should implement the same renderer abstraction.

### Multi-Window Support

The architecture should support multiple windows without creating a separate global renderer for every window.

### Extensibility

New backends and renderers should be addable without modifying the core API.

For example:

```text
New Platform:
    Wayland
    X11
    └── Future Backend

New Renderer:
    OpenGL
    Vulkan
    └── Future Renderer
```

The ultimate goal is for Walrus UI code to remain completely unaware of which platform or graphics API is being used.
