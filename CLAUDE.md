# Walrus

Walrus is a native Linux GUI toolkit written in C.

The project aims to provide a modular, low-level, extensible GUI toolkit
capable of building real desktop applications on Linux.

## Core Goals

- Native Linux desktop GUI toolkit
- C11/C17
- Wayland-first
- X11 fallback
- EGL + OpenGL rendering
- Modular architecture
- Explicit resource ownership
- Minimal dependencies
- Stable and clean public API
- Buildable and testable at every development stage

## Target Platform

Primary target:

- Linux x86_64
- Wayland
- X11

Graphics stack:

- EGL
- OpenGL

Build system:

- CMake

## Repository Structure

Expected architecture:

```text
walrus/
├── CMakeLists.txt
├── include/
│   └── walrus/
│       ├── walrus.h
│       ├── window.h
│       ├── event.h
│       ├── renderer.h
│       ├── widget.h
│       └── types.h
│
├── src/
│   ├── core/
│   ├── backend/
│   │   ├── wayland/
│   │   └── x11/
│   ├── renderer/
│   │   └── opengl/
│   └── widgets/
│
├── external/
├── examples/
├── tests/
└── docs/
```

Do not assume every directory or subsystem exists yet.
Inspect the current repository before modifying or extending it.

## Architecture Rules

Keep the architecture layered:

```text
Public API
    ↓
Core
    ↓
Backend abstraction
    ├── Wayland
    └── X11

Core
    ↓
Renderer abstraction
    ↓
EGL
    ↓
OpenGL
```

Core code must not depend directly on Wayland or X11 implementation details.

Platform-specific code belongs inside its backend.

Rendering implementation details must not leak into the public API unless
there is a strong architectural reason.

Prefer opaque public types:

```c
typedef struct WalrusWindow WalrusWindow;
typedef struct WalrusRenderer WalrusRenderer;
typedef struct WalrusWidget WalrusWidget;
```

over exposing internal implementation structures.

## Backend Selection

Prefer Wayland when `WAYLAND_DISPLAY` is available.

Fallback to X11 when Wayland is unavailable and `DISPLAY` is available.

Backend selection must remain hidden behind the backend abstraction.

The application should not need to know whether it is running on Wayland or X11.

## Wayland

The Wayland backend should eventually handle:

- wl_display
- wl_registry
- wl_compositor
- wl_surface
- xdg_wm_base
- xdg_surface
- xdg_toplevel
- configure events
- close events
- resize
- pointer input
- keyboard input
- frame callbacks

Do not add protocol complexity until the current window lifecycle is stable.

## X11

The X11 backend should eventually handle:

- window creation
- window destruction
- resize
- close events
- keyboard input
- pointer input
- event polling

Use one X11 API consistently rather than mixing incompatible approaches.

## Rendering

Rendering architecture:

```text
Walrus Renderer
       ↓
      EGL
       ↓
    OpenGL
```

Renderer responsibilities should include:

- context initialization
- context destruction
- frame lifecycle
- viewport management
- clearing
- buffer presentation

Prefer a renderer abstraction so another graphics backend could be added later.

## Event System

Use a unified event representation independent of the native backend.

Example:

```c
typedef enum {
    WALRUS_EVENT_NONE,
    WALRUS_EVENT_QUIT,
    WALRUS_EVENT_RESIZE,
    WALRUS_EVENT_KEY_DOWN,
    WALRUS_EVENT_KEY_UP,
    WALRUS_EVENT_MOUSE_MOVE,
    WALRUS_EVENT_MOUSE_BUTTON_DOWN,
    WALRUS_EVENT_MOUSE_BUTTON_UP
} WalrusEventType;
```

Wayland and X11 events should be translated into Walrus events.

Do not expose native event structures through the high-level API unless
explicitly required.

## Widget System

Widgets should form a tree:

```text
Window
└── Root Container
    ├── Panel
    │   ├── Label
    │   └── Button
    └── TextInput
```

Implement incrementally:

1. Base widget
2. Container
3. Label
4. Button
5. Text input
6. Panel
7. Scroll container
8. Layout system

Do not implement a large widget framework before the underlying window,
event, and rendering systems are reliable.

## Memory Management

Walrus does not use garbage collection.

Every owned resource must have a clear lifecycle:

```text
create
  ↓
initialize
  ↓
use
  ↓
destroy
```

Avoid:

- memory leaks
- double frees
- use-after-free
- dangling pointers
- ownership ambiguity

Prefer explicit ownership over hidden reference counting unless reference
counting provides a clear architectural benefit.

System resources such as:

- Wayland objects
- X11 resources
- EGL objects
- OpenGL objects
- file descriptors

must have clearly defined ownership and destruction paths.

## Coding Rules

- Use C11 unless the existing project explicitly requires otherwise.
- Keep compiler warnings enabled.
- Do not silence warnings without understanding them.
- Check important return values.
- Validate external/system API results.
- Avoid unnecessary global state.
- Avoid unnecessary heap allocations.
- Keep public headers minimal.
- Keep implementation details private.
- Avoid premature abstraction.
- Prefer simple, explicit code.
- Do not introduce C++ dependencies.
- Keep third-party dependencies isolated under `external/`.

## Naming Convention

Types:

```c
WalrusWindow
WalrusEvent
WalrusRenderer
```

Functions:

```c
walrus_window_create()
walrus_window_destroy()
walrus_renderer_init()
walrus_renderer_shutdown()
```

Constants and enum values:

```c
WALRUS_EVENT_QUIT
WALRUS_EVENT_RESIZE
```

## Public API

Public headers are part of Walrus's API.

Avoid exposing implementation details.

Prefer:

```c
typedef struct WalrusWindow WalrusWindow;
```

instead of:

```c
struct WalrusWindow {
    ...
};
```

in public headers.

Public APIs should have predictable ownership semantics.

If a function returns an allocated object, document who owns it and how it
must be destroyed.

## Build

Standard configure:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

Clean rebuild:

```bash
rm -rf build
cmake -S . -B build
cmake --build build
```

Install:

```bash
cmake --install build
```

When possible, provide a CMake package target:

```cmake
find_package(Walrus REQUIRED)
target_link_libraries(app PRIVATE Walrus::Walrus)
```

## Debugging

Use Debug builds while investigating crashes or runtime problems.

Use sanitizers when appropriate:

```text
-fsanitize=address
-fsanitize=undefined
```

For crashes:

1. Reproduce the issue.
2. Identify the failing layer.
3. Inspect the ownership/lifecycle involved.
4. Make the smallest correct fix.
5. Rebuild.
6. Re-run the reproducer.
7. Run relevant tests.

Do not hide crashes by disabling assertions, removing checks, or ignoring
error codes.

## CMake Rules

Keep platform dependencies explicit.

Examples of dependency categories:

- Wayland libraries
- X11 libraries
- EGL
- OpenGL

Do not add linker flags blindly to solve linker errors.

When fixing linker errors, determine:

1. Which symbol is missing.
2. Which library provides it.
3. Whether headers and libraries match.
4. Whether the correct target is linked.
5. Whether link order or transitive dependencies matter.

Prefer target-based CMake:

```cmake
target_link_libraries(...)
target_include_directories(...)
target_compile_features(...)
```

over global compiler/linker flags.

## Development Workflow

Before changing code:

1. Inspect the existing repository.
2. Understand the current architecture.
3. Find the smallest appropriate change.
4. Implement incrementally.
5. Build the project.
6. Run relevant tests/examples.
7. Verify the result.

Do not rewrite working subsystems without a clear reason.

Do not invent files, APIs, or abstractions that are not present in the
repository.

If an architectural change is necessary, explain why before making a large
rewrite.

## Development Phases

Build Walrus incrementally:

### Phase 1 — Foundation

- CMake
- public headers
- core lifecycle
- backend abstraction

### Phase 2 — Wayland

- display connection
- registry
- compositor
- xdg-shell
- window creation
- window destruction

### Phase 3 — X11

- display connection
- window creation
- event polling
- window destruction

### Phase 4 — EGL

- EGL display
- EGL configuration
- EGL surface
- EGL context

### Phase 5 — OpenGL

- OpenGL context
- frame lifecycle
- clear
- swap/presentation

### Phase 6 — Unified Events

- keyboard
- mouse
- resize
- close
- event queue

### Phase 7 — Widget Tree

- widget lifecycle
- hierarchy
- event propagation
- rendering traversal

### Phase 8 — Basic Widgets

- container
- label
- button
- text input
- panel

### Phase 9 — Layout

- sizing
- positioning
- parent/child layout
- basic constraints

### Phase 10 — Text

- font loading
- glyph rendering
- text measurement

### Phase 11 — Styling

- colors
- typography
- spacing
- borders
- themes

### Phase 12 — Packaging

- installation
- CMake package
- examples
- documentation
- tests

Every phase should leave the repository buildable.

## Testing

Tests should eventually cover:

- core lifecycle
- backend selection
- event translation
- widget tree
- ownership/lifecycle
- renderer initialization

Examples are also important integration tests.

At minimum, maintain a simple hello-world application that creates a
window and renders something.

## Error Handling

When fixing an error, always determine the root cause rather than treating
the first visible error as the actual problem.

For each bug:

- identify the subsystem
- identify the ownership/lifecycle involved
- identify the root cause
- make the smallest correct change
- verify the fix

Do not perform unrelated refactors while fixing a bug.

## Dependencies

Keep dependencies minimal.

Before introducing a new dependency:

1. Determine whether it is actually necessary.
2. Check whether existing system APIs already provide the required feature.
3. Explain the architectural trade-off.
4. Isolate third-party code where practical.

Do not add a dependency simply for convenience.

## Current Priority

Prioritize the lowest incomplete phase.

Do not jump to advanced widgets, styling, or text rendering while the
backend, event, or renderer foundations are unstable.

The end goal is a genuinely usable native Linux GUI toolkit, not merely an
OpenGL window demo.

## Working With Claude Code

When asked to implement something:

1. Inspect the repository first.
2. Read relevant existing source files.
3. Reuse existing architecture where possible.
4. State the intended change briefly.
5. Modify only the necessary files.
6. Build and test.
7. Report what changed and whether verification passed.

When the user provides compiler errors, linker errors, runtime logs, or
source code, treat those as primary evidence.

Do not assume the repository matches an example architecture perfectly.

If the actual repository differs from this document, the existing working
code takes precedence and this document should be updated when the
architecture intentionally changes.
