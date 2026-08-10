# Walrus Wayland Backend

Walrus uses the Wayland client protocol as one of its window-system backends.

The Wayland backend is responsible for communicating with the Wayland compositor, creating native surfaces, handling window events, and receiving input events.

The backend should not contain rendering logic.

---

# Architecture

The Wayland backend sits between the Walrus core and the Wayland compositor.

```text
                    Walrus Core
                        │
                        ▼
                  WrBackend
                        │
                        ▼
                  Wayland Backend
                        │
          ┌─────────────┼─────────────┐
          │             │             │
     wl_display    wl_compositor   xdg_wm_base
          │             │             │
          └─────────────┼─────────────┘
                        │
                        ▼
                  Wayland Compositor
```

The backend is responsible for the native window system, while rendering is handled separately.

```text
Wayland Backend
      │
      └── wl_surface
              │
              ▼
        Render Surface
              │
              ▼
          Renderer
```

---

# Connection to the Compositor

The first step is connecting to the Wayland compositor.

```c
struct wl_display *display;

display = wl_display_connect(NULL);
```

Conceptually:

```text
Walrus
  │
  │ wl_display_connect()
  ▼
Wayland Compositor
```

If the connection fails, backend initialization must fail.

---

# Global Registry

After connecting, the client obtains the global registry.

```c
struct wl_registry *registry;

registry = wl_display_get_registry(display);
```

The registry contains the interfaces advertised by the compositor.

Examples:

```text
wl_compositor
wl_shm
wl_seat
wl_output
xdg_wm_base
```

Walrus binds only to interfaces required by the backend.

---

# Registry Discovery

The client registers a listener:

```c
wl_registry_add_listener(
    registry,
    &registry_listener,
    NULL
);
```

The compositor then announces available globals through:

```c
static void registry_add(
    void *data,
    struct wl_registry *registry,
    uint32_t name,
    const char *interface,
    uint32_t version
);
```

For example:

```text
registry_add()
      │
      ├── wl_compositor
      │
      ├── xdg_wm_base
      │
      ├── wl_seat
      │
      └── wl_output
```

The client binds to the interfaces it needs.

---

# Required Wayland Interfaces

A basic desktop window requires at least:

```text
wl_display
wl_registry
wl_compositor
xdg_wm_base
```

The relationships are:

```text
wl_display
    │
    ▼
wl_registry
    │
    ├── wl_compositor
    │
    └── xdg_wm_base
```

`wl_compositor` is used to create `wl_surface`.

`xdg_wm_base` is used to turn that surface into a desktop toplevel window.

---

# Creating a Window

Wayland separates the concept of a surface from the concept of a desktop window.

The basic creation flow is:

```text
                 WrWindow
                    │
                    ▼
              wl_surface
                    │
                    ▼
              xdg_surface
                    │
                    ▼
              xdg_toplevel
                    │
                    ▼
             Desktop Window
```

---

# `wl_surface`

A surface is created using `wl_compositor`.

```c
struct wl_surface *surface;

surface = wl_compositor_create_surface(
    compositor
);
```

The surface represents an area that can receive graphical content.

It does not automatically become a normal desktop window.

---

# `xdg_surface`

The `wl_surface` is wrapped in an `xdg_surface`.

```c
struct xdg_surface *xdg_surface;

xdg_surface = xdg_wm_base_get_xdg_surface(
    wm_base,
    surface
);
```

`xdg_surface` provides the protocol needed for desktop surface configuration.

---

# `xdg_toplevel`

To create a normal top-level application window:

```c
struct xdg_toplevel *xdg_toplevel;

xdg_toplevel = xdg_surface_get_toplevel(
    xdg_surface
);
```

The resulting relationship is:

```text
wl_surface
    │
    ▼
xdg_surface
    │
    ▼
xdg_toplevel
```

The title can then be set:

```c
xdg_toplevel_set_title(
    xdg_toplevel,
    "Walrus"
);
```

---

# Initial Commit

Wayland requires the client to commit the surface before the compositor can send the initial configuration.

```c
wl_surface_commit(surface);
```

At this point, the client should not assume the final window size.

The compositor will send an `xdg_surface.configure` event.

---

# Configure Event

The compositor sends:

```c
xdg_surface.configure(
    surface,
    serial
);
```

The client must acknowledge the configure event:

```c
xdg_surface_ack_configure(
    surface,
    serial
);
```

The general sequence is:

```text
Create Surface
      │
      ▼
Create xdg_surface
      │
      ▼
Create xdg_toplevel
      │
      ▼
Commit
      │
      ▼
Compositor
      │
      ▼
configure event
      │
      ▼
ack_configure()
      │
      ▼
Render / Commit
```

This configure/acknowledge cycle is fundamental to correct Wayland window management.

---

# Window Size

Wayland does not work like X11 where an application can simply assume that its requested size is the final size.

The compositor controls the actual window configuration.

The `xdg_toplevel.configure` event provides information about the current size and states.

Conceptually:

```c
static void xdg_toplevel_configure(
    void *data,
    struct xdg_toplevel *toplevel,
    int32_t width,
    int32_t height,
    struct wl_array *states
);
```

The compositor may provide:

```text
width  = 0
height = 0
```

which means the client should choose a suitable size.

Otherwise, the compositor may provide a size that the client should use.

---

# Window States

The compositor can provide states such as:

```text
activated
maximized
fullscreen
resizing
```

The backend should translate these into Walrus-level window state when necessary.

The core should not directly depend on Wayland protocol states.

---

# Rendering Relationship

The Wayland backend does not render the window itself.

It creates the native `wl_surface`.

The renderer then connects the native surface to the graphics API.

For OpenGL:

```text
Wayland
   │
   ▼
wl_surface
   │
   ▼
EGL
   │
   ▼
EGLSurface
   │
   ▼
OpenGL
   │
   ▼
GPU
```

For Vulkan:

```text
Wayland
   │
   ▼
wl_surface
   │
   ▼
VkSurfaceKHR
   │
   ▼
Vulkan
   │
   ▼
GPU
```

This separation allows the same Wayland backend to work with multiple renderers.

---

# OpenGL + Wayland

For an OpenGL renderer, EGL is used to connect OpenGL with the Wayland surface.

Conceptually:

```text
wl_display
    │
    ▼
EGLDisplay
    │
    ├── EGLConfig
    ├── EGLContext
    │
    └── EGLSurface
            │
            ▼
        wl_surface
```

The renderer owns EGL objects.

The Wayland backend owns the Wayland objects.

---

# Frame Rendering

A simplified OpenGL frame looks like:

```text
Wayland Window
      │
      ▼
EGLSurface
      │
      ▼
eglMakeCurrent()
      │
      ▼
OpenGL commands
      │
      ▼
GPU
      │
      ▼
eglSwapBuffers()
      │
      ▼
Wayland Compositor
```

The compositor then decides when and where the surface is displayed.

---

# Frame Callbacks

A Wayland client should not blindly render as fast as possible.

The compositor can provide a frame callback:

```c
wl_surface_frame(surface);
```

The callback tells the client when it is appropriate to produce another frame.

Conceptually:

```text
Render Frame
    │
    ▼
Commit Surface
    │
    ▼
Request Frame Callback
    │
    ▼
Compositor
    │
    ▼
Frame Callback
    │
    ▼
Render Next Frame
```

This can be used to synchronize rendering with the compositor.

---

# Event Loop

Wayland is event-driven.

Walrus should process Wayland events through its backend.

A simplified loop is:

```text
while (!window_should_close)
{
    poll_events();

    update();

    render();
}
```

The Wayland backend may internally use:

```c
wl_display_dispatch();
wl_display_dispatch_pending();
wl_display_flush();
```

The exact event-loop integration should eventually support blocking when no rendering or application work is required.

---

# Event Processing

Events arrive from the Wayland compositor.

Examples:

```text
Window configure
Window close
Pointer motion
Pointer button
Keyboard key
Keyboard modifiers
Output changes
```

The backend receives these events through protocol listeners.

The backend then converts them into Walrus events.

```text
Wayland Event
     │
     ▼
Wayland Listener
     │
     ▼
Walrus Event
     │
     ▼
Core Event System
     │
     ▼
Application
```

---

# Input

Input devices are exposed through `wl_seat`.

```text
wl_seat
   │
   ├── wl_pointer
   │
   ├── wl_keyboard
   │
   └── wl_touch
```

The Wayland backend should translate these events into the generic Walrus input system.

For example:

```text
wl_pointer.motion
        │
        ▼
WR_EVENT_MOUSE_MOVE
```

and:

```text
wl_keyboard.key
        │
        ▼
WR_EVENT_KEY
```

The core input system should not need to know that the event originated from Wayland.

---

# Pointer Input

Pointer events can include:

```text
motion
button
axis
enter
leave
```

The backend should convert coordinates and button states into the Walrus input representation.

---

# Keyboard Input

Keyboard input is provided through `wl_keyboard`.

Important events include:

```text
keymap
enter
leave
key
modifiers
repeat_info
```

Keyboard handling should eventually include:

* physical key state
* modifiers
* key repeat
* text input

Text input may require additional Wayland protocols beyond the basic keyboard interface.

---

# Window Closing

A top-level window can receive:

```c
xdg_toplevel.close
```

The backend should not immediately destroy the window unless that is the desired API behavior.

Instead, it should update the core window state:

```text
xdg_toplevel.close
        │
        ▼
window->should_close = 1
```

The application can then decide when to destroy the window.

---

# Window Destruction

A window should be destroyed in the reverse order of creation.

```text
xdg_toplevel
      │
      ▼
xdg_surface
      │
      ▼
wl_surface
      │
      ▼
Backend Data
```

The backend should release all protocol objects before freeing its private data.

---

# Threading

A basic Walrus Wayland backend should initially use one thread for:

```text
Wayland event processing
Window management
Rendering
```

Wayland objects generally require careful event-queue and thread handling.

Multi-threaded rendering can be introduced later if necessary.

The initial architecture should favor correctness and simplicity.

---

# Backend Responsibilities

The Wayland backend is responsible for:

```text
✓ Connect to compositor
✓ Discover globals
✓ Bind required protocols
✓ Create wl_surface
✓ Create xdg_surface
✓ Create xdg_toplevel
✓ Set title
✓ Process configure events
✓ Process window events
✓ Process input events
✓ Destroy Wayland objects
```

The Wayland backend is **not** responsible for:

```text
✗ OpenGL commands
✗ EGL context creation
✗ Vulkan rendering
✗ Shader compilation
✗ Vertex buffers
✗ UI layout
✗ UI drawing
```

Those responsibilities belong to the appropriate layers.

---

# Overall Flow

The complete lifecycle is:

```text
                    Application
                         │
                         ▼
                  wr_create_window()
                         │
                         ▼
                     WrWindow
                         │
                         ▼
                    WrBackend
                         │
                         ▼
                 Wayland Backend
                         │
             ┌───────────┴───────────┐
             │                       │
             ▼                       ▼
        wl_surface              xdg_toplevel
             │                       │
             └───────────┬───────────┘
                         │
                         ▼
                    Render Surface
                         │
                         ▼
                     Renderer
                         │
                    ┌────┴────┐
                    │         │
                  OpenGL    Vulkan
                    │         │
                   EGL       Vulkan
                    │         │
                    └────┬────┘
                         │
                         ▼
                        GPU
                         │
                         ▼
                  Wayland Compositor
```

The key architectural rule is:

> **Wayland manages the window and input; the renderer manages graphics; the core manages application-facing state.**
