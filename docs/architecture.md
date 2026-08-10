# Walrus Architecture

Walrus is designed as a modular C GUI toolkit for Linux. Its architecture separates the public API, core window/input system, platform backends, and rendering implementations.

The main goal is to keep platform-specific and graphics-API-specific code isolated from the higher-level UI system.

## Architecture Overview

```text
                         ┌──────────────────┐
                         │    Walrus API    │
                         │   walrus.h       │
                         └────────┬─────────┘
                                  │
                    ┌─────────────▼─────────────┐
                    │           Core            │
                    │                           │
                    │  Window / Event / Input   │
                    └─────────────┬─────────────┘
                                  │
                    ┌─────────────▼─────────────┐
                    │          Backend          │
                    │                           │
                    │   Wayland  │  X11         │
                    └─────────────┬─────────────┘
                                  │
                           Native Surface
                                  │
                    ┌─────────────▼─────────────┐
                    │         Renderer          │
                    │                           │
                    │ OpenGL │ Vulkan │ Custom  │
                    └───────────────────────────┘
```

Each layer has a specific responsibility and should avoid depending on implementation details from other layers.

---

# 1. Public API

The public API is exposed through `walrus.h` and other public headers.

Applications should interact with Walrus through this layer instead of directly accessing Wayland, X11, EGL, or OpenGL.

```text
Application
     │
     ▼
Walrus API
```

The public API should remain stable even if the internal backend or renderer changes.

---

# 2. Core Layer

The core layer contains platform-independent concepts.

```text
include/walrus/core/
src/core/
```

Examples:

* `WrWindow`
* events
* input
* application state
* window lifecycle

The core layer should not contain:

```c
struct wl_surface;
EGLSurface;
VkSurfaceKHR;
Display;
Window;
```

Instead, platform-specific data should remain behind an abstraction.

For example:

```c
typedef struct WrWindow {
    char *title;
    int width;
    int height;

    int should_close;

    void *backend_data;
} WrWindow;
```

The core knows that backend data exists, but does not need to know its implementation.

---

# 3. Backend Layer

The backend layer handles communication with the operating-system window system.

```text
include/walrus/backend/
src/backend/
```

Current backends:

```text
Wayland
X11
```

The backend is responsible for:

* connecting to the display server
* discovering required protocols
* creating native windows/surfaces
* destroying native windows
* processing platform events
* receiving platform input events

The backend should expose a common interface.

Conceptually:

```c
typedef struct WrBackend {
    int (*init)(void);
    void (*shutdown)(void);

    void (*poll_events)(void);

    void *(*create_window)(
        char *title
    );

    void (*destroy_window)(
        void *data
    );
} WrBackend;
```

The implementation can then differ:

```text
WrBackend
   │
   ├── Wayland
   │     └── wl_display
   │         wl_surface
   │         xdg_surface
   │         xdg_toplevel
   │
   └── X11
         └── Display
             Window
```

The rest of Walrus does not need to know which backend is active.

---

# 4. Renderer Layer

The renderer layer provides an abstraction over graphics APIs.

```text
include/walrus/renderer/
src/renderer/
```

Possible implementations:

```text
OpenGL
```

The renderer should not be tightly coupled to a specific window-system backend.

Conceptually:

```text
                 WrRenderer
                     │
          ┌──────────┼──────────┐
          │          │          │
       OpenGL      Vulkan     Custom
```

The renderer interface can contain operations such as:

```c
typedef struct WrRenderer {
    int (*init)(void);
    void (*shutdown)(void);

    WrRenderSurface *(*create_surface)(
        WrWindow *window
    );

    void (*destroy_surface)(
        WrRenderSurface *surface
    );

    int (*begin_frame)(
        WrRenderSurface *surface
    );

    int (*end_frame)(
        WrRenderSurface *surface
    );

    void (*draw_batch)(
        WrRenderSurface *surface,
        WrBatch *batch
    );
} WrRenderer;
```

The interface should remain independent of OpenGL and Vulkan.

---

# 5. Render Surface

A render surface represents the connection between a Walrus window and a renderer.

It should not expose graphics API-specific types.

Conceptually:

```c
typedef struct WrRenderSurface {
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
