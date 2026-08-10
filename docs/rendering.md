# Walrus Rendering

Walrus uses a renderer abstraction to separate the UI system from the underlying graphics API.

The renderer is responsible for converting UI and application rendering data into GPU commands.

The initial renderer implementation is OpenGL, with Vulkan and other rendering backends planned for the future.

---

## Rendering Architecture

```text
                     UI / Application
                            │
                            ▼
                       Render Data
                            │
                            ▼
                         WrBatch
                            │
                            ▼
                       WrRenderer
                            │
              ┌─────────────┼─────────────┐
              │             │             │
           OpenGL         Vulkan        Custom
              │
              ▼
             EGL
              │
              ▼
          OpenGL Context
              │
              ▼
             GLAD
              │
              ▼
             GPU
```

The important rule is that `WrRenderer` does not expose OpenGL, EGL, or Vulkan types.

---

# Renderer

`WrRenderer` is the common interface implemented by every rendering backend.

Conceptually:

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

The renderer implementation is selected internally by Walrus.

For example:

```text
WrRenderer
    │
    ├── wr_opengl_renderer
    │
    ├── wr_vulkan_renderer
    │
    └── wr_custom_renderer
```

The rest of Walrus only communicates through `WrRenderer`.

---

# Render Surface

A render surface represents the renderer-specific connection between a `WrWindow` and the graphics API.

The window itself should not store API-specific objects such as:

```c
EGLSurface
VkSurfaceKHR
```

Instead:

```text
WrWindow
    │
    ▼
WrRenderSurface
    │
    └── renderer_data
```

For OpenGL:

```text
WrRenderSurface
    │
    └── OpenGL surface data
            │
            └── EGLSurface
```

For Vulkan:

```text
WrRenderSurface
    │
    └── Vulkan surface data
            │
            └── VkSurfaceKHR
```

This keeps `WrWindow` independent from the rendering API.

---

# Rendering a Frame

A typical frame follows this sequence:

```text
┌─────────────────────┐
│ Update application  │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ Update UI state     │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ Generate WrBatch    │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ begin_frame()       │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ draw_batch()        │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│ end_frame()         │
└─────────────────────┘
```

The application should not need to manually call graphics API functions.

---

# OpenGL Renderer

The first rendering backend uses OpenGL.

```text
renderer/opengl/
├── opengl.h
├── egl.h
├── vertex.h
├── batch.h
└── shader.h
```

The implementation lives under:

```text
src/renderer/opengl/
├── opengl.c
└── egl.c
```

The OpenGL renderer consists of several layers.

```text
WrRenderer
     │
     ▼
OpenGL Renderer
     │
     ├── EGL
     ├── GLAD
     ├── Shader
     ├── Vertex
     └── Batch
```

---

# EGL

EGL is responsible for connecting OpenGL to the native window system.

It handles things such as:

* EGL display
* EGL configuration
* OpenGL context
* rendering surfaces
* making a context current
* presenting rendered frames

The relationship is:

```text
Wayland / X11
       │
       ▼
      EGL
       │
       ▼
OpenGL Context
       │
       ▼
    OpenGL API
```

EGL is an implementation detail of the OpenGL renderer.

The generic renderer interface should never expose EGL types.

---

# GLAD

GLAD is used to load OpenGL functions.

For example:

```c
glCreateShader();
glCreateProgram();
glGenBuffers();
glBindBuffer();
glBufferData();
glDrawElements();
```

GLAD does not create windows, surfaces, or contexts.

Its responsibility is loading OpenGL function pointers after an appropriate OpenGL context has been created.

Therefore:

```text
EGL
 └── creates OpenGL context

GLAD
 └── loads OpenGL functions
```

---

# OpenGL Context

The OpenGL context contains the state required for OpenGL rendering.

A simplified initialization sequence is:

```text
EGLDisplay
    │
    ▼
eglInitialize()
    │
    ▼
Choose EGLConfig
    │
    ▼
Create EGLContext
    │
    ▼
Create EGLSurface
    │
    ▼
eglMakeCurrent()
    │
    ▼
Load OpenGL functions with GLAD
```

Only after the context is current should OpenGL functions be used.

---

# Multi-Window Rendering

Walrus is designed to support multiple windows.

Conceptually:

```text
                 WrRenderer
                     │
              Shared renderer
                     │
        ┌────────────┼────────────┐
        │            │            │
     Window A     Window B     Window C
        │            │            │
   Surface A     Surface B     Surface C
```

The renderer may share resources such as:

* shaders
* textures
* vertex buffers
* index buffers
* pipelines

while each window owns its own render surface.

For OpenGL, this commonly means:

```text
Shared:
    EGLDisplay
    EGLContext
    EGLConfig

Per Window:
    EGLSurface
```

The exact resource-sharing implementation remains renderer-specific.

---

# Vertex Data

Rendering UI elements eventually produces geometry.

A vertex can contain information such as:

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

The actual vertex format may evolve as rendering requirements increase.

For example, future UI rendering may require:

* position
* color
* texture coordinates
* clipping information
* transform information
* custom attributes

---

# Indices

Instead of duplicating vertices, indexed rendering can be used.

Example:

```text
Vertices:

0 ───── 1
│       │
│       │
│       │
2 ───── 3

Indices:

0, 1, 2
2, 1, 3
```

This allows multiple triangles to reuse the same vertices.

---

# Batching

Walrus should use batching to reduce draw calls.

Instead of:

```text
Button → draw call
Text   → draw call
Image  → draw call
Shape  → draw call
```

the renderer can combine compatible geometry:

```text
Button ─┐
Text   ─┤
Image  ─┼──→ WrBatch → GPU
Shape  ─┘
```

A batch may contain:

```text
WrBatch
 ├── vertices
 ├── indices
 ├── textures
 └── draw information
```

The exact batching strategy depends on the renderer.

---

# Shader System

OpenGL rendering uses shaders to determine how geometry is processed.

The basic pipeline is:

```text
Vertex Data
    │
    ▼
Vertex Shader
    │
    ▼
Rasterization
    │
    ▼
Fragment Shader
    │
    ▼
Framebuffer
```

Walrus should keep shader management inside the renderer implementation.

The core UI system should not need to know about:

```c
GLuint
GLenum
GLSL
```

---

# Presentation

After rendering is complete, the renderer presents the frame.

For OpenGL/EGL:

```text
Render
   │
   ▼
eglSwapBuffers()
   │
   ▼
Native Window
```

For Vulkan:

```text
Render
   │
   ▼
Submit Command Buffer
   │
   ▼
Present Swapchain Image
```

Both can therefore implement the same high-level operation:

```c
renderer->end_frame(surface);
```

without exposing their internal mechanisms.

---

# Renderer Independence

The following code should remain renderer-independent:

```text
UI
Core
Window
Input
Event System
Layout
Widgets
```

The following code is renderer-specific:

```text
OpenGL
EGL
GLAD
GLSL
Vulkan
SPIR-V
VkSurfaceKHR
```

This separation allows the same UI system to eventually render through different graphics APIs.

---

# Future Renderers

The architecture is intended to support additional renderers.

Possible implementations:

```text
OpenGL
Vulkan
Software Renderer
DirectX
Metal
Custom GPU Renderer
```

Adding a renderer should primarily require implementing the `WrRenderer` interface.

For example:

```text
WrRenderer
    │
    ├── OpenGL
    ├── Vulkan
    ├── Software
    └── Custom
```

The core UI code should not need to change when switching between these implementations.

---

# Design Rule

The most important rule of the rendering architecture is:

> **The core system describes what should be rendered; the renderer decides how it is rendered.**

The UI system generates rendering data.

The renderer converts that data into graphics API commands.

This keeps Walrus modular and allows the rendering implementation to evolve independently from the rest of the toolkit.
