# Walrus Input System

Walrus uses an input abstraction to provide a platform-independent interface for keyboard, pointer, and other input devices.

The input system separates platform-specific input events from the application.

A Wayland or X11 backend receives native input events and converts them into Walrus events.

---

# Architecture

```text
                Platform
                   │
          ┌────────┴────────┐
          │                 │
       Wayland              X11
          │                 │
          ▼                 ▼
   Native Input Events
          │
          ▼
      WrEvent
          │
          ▼
     WrInput System
          │
          ▼
      Application
```

The application should never need to directly interact with:

```c
wl_pointer
wl_keyboard
XEvent
```

Instead, those platform-specific events are converted into Walrus events.

---

# Input Layers

The input architecture consists of three main layers:

```text
Backend
   │
   ▼
Event System
   │
   ▼
Input State
```

### Backend

Receives native platform events.

Examples:

```text
Wayland:
    wl_pointer
    wl_keyboard
    wl_touch

X11:
    XEvent
```

### Event System

Converts platform events into generic Walrus events.

### Input State

Maintains the current state of devices.

For example:

```text
Keyboard:
    pressed / released

Mouse:
    position
    buttons
    scroll

Touch:
    active points
```

---

# Event Flow

A typical input event flows through the system like this:

```text
Keyboard
    │
    ▼
Wayland / X11
    │
    ▼
Backend
    │
    ▼
WrEvent
    │
    ▼
Event Queue
    │
    ▼
Input System
    │
    ▼
Application / UI
```

The same application code can therefore receive input regardless of the underlying window system.

---

# WrEvent

`WrEvent` represents a generic event inside Walrus.

A simplified design could be:

```c
typedef enum WrEventType {
    WR_EVENT_NONE,

    WR_EVENT_WINDOW_CLOSE,
    WR_EVENT_WINDOW_RESIZE,

    WR_EVENT_KEY_PRESS,
    WR_EVENT_KEY_RELEASE,

    WR_EVENT_MOUSE_MOVE,
    WR_EVENT_MOUSE_BUTTON_PRESS,
    WR_EVENT_MOUSE_BUTTON_RELEASE,
    WR_EVENT_MOUSE_SCROLL,

    WR_EVENT_TEXT_INPUT
} WrEventType;
```

An event can then contain event-specific information.

For example:

```c
typedef struct WrEvent {
    WrEventType type;

    union {
        struct {
            int width;
            int height;
        } resize;

        struct {
            int key;
            int modifiers;
        } key;

        struct {
            double x;
            double y;
        } mouse_move;

        struct {
            int button;
            int modifiers;
        } mouse_button;

        struct {
            double x;
            double y;
        } scroll;
    };
} WrEvent;
```

The exact event structure can evolve as more input devices are supported.

---

# Event Queue

Input events should be processed through an event queue.

Conceptually:

```text
Backend
   │
   ▼
Create WrEvent
   │
   ▼
Push Event
   │
   ▼
Event Queue
   │
   ▼
Application
```

This prevents platform-specific event processing from being directly coupled to application code.

A queue might conceptually contain:

```text
┌─────────────────────────┐
│ Event 1                 │
├─────────────────────────┤
│ Event 2                 │
├─────────────────────────┤
│ Event 3                 │
├─────────────────────────┤
│ Event 4                 │
└─────────────────────────┘
```

Events are processed in the order they are received.

---

# Keyboard Input

Keyboard input is represented using generic key events.

```text
Platform
   │
   ▼
Key Press
   │
   ▼
WR_EVENT_KEY_PRESS
```

A key event should contain enough information to distinguish keys and modifiers.

For example:

```c
typedef struct WrKeyEvent {
    int key;
    int modifiers;
} WrKeyEvent;
```

The key representation should be platform-independent.

The backend performs the translation:

```text
Wayland key code
       │
       ▼
Walrus key code
```

and:

```text
X11 key code
       │
       ▼
Walrus key code
```

This allows the application to use one key representation.

---

# Key State

The input system should maintain the current state of keys.

Conceptually:

```c
bool wr_key_down(WrKey key);
```

The application can then query:

```c
if (wr_key_down(WR_KEY_SPACE))
{
    // Space is currently held.
}
```

This is different from an event.

An event tells the application:

```text
"Space was pressed."
```

Input state tells the application:

```text
"Space is currently held."
```

Both systems are useful.

---

# Mouse / Pointer Input

Pointer input contains:

```text
position
buttons
scroll
enter / leave
```

A pointer movement event may look like:

```c
typedef struct WrMouseMoveEvent {
    double x;
    double y;
} WrMouseMoveEvent;
```

The input state can store:

```text
Mouse X
Mouse Y
Button states
```

---

# Mouse Buttons

Mouse buttons should use platform-independent identifiers.

For example:

```c
typedef enum WrMouseButton {
    WR_MOUSE_BUTTON_LEFT,
    WR_MOUSE_BUTTON_RIGHT,
    WR_MOUSE_BUTTON_MIDDLE
} WrMouseButton;
```

A platform backend translates native button codes into these values.

---

# Mouse Position

The current pointer position can be queried independently of events.

Conceptually:

```c
double wr_mouse_x(void);
double wr_mouse_y(void);
```

This is useful for:

* UI interaction
* dragging
* game input
* cursor interaction
* hit testing

---

# Scroll

Scroll input is represented independently from pointer movement.

```c
typedef struct WrScrollEvent {
    double x;
    double y;
} WrScrollEvent;
```

For example:

```text
Mouse Wheel
     │
     ▼
Wayland axis event
     │
     ▼
WR_EVENT_MOUSE_SCROLL
     │
     ▼
Input System
```

---

# Touch Input

Touch support can be added through the same abstraction.

Possible events include:

```text
touch down
touch motion
touch up
```

Conceptually:

```c
typedef struct WrTouchPoint {
    int id;

    double x;
    double y;
} WrTouchPoint;
```

A touch event should identify the individual touch point.

---

# Text Input

Keyboard keys and text input should be treated as separate concepts.

For example:

```text
Key event:
    User pressed a physical key.

Text event:
    User produced a character.
```

This distinction is important for:

* text fields
* IME
* international keyboards
* composition
* dead keys
* accented characters

Wayland text input may require protocols such as:

```text
zwp_text_input_v3
```

This should be implemented separately from basic keyboard input.

---

# Event vs Input State

These two concepts should not be mixed.

## Event

Describes something that happened.

```text
KEY_PRESS
MOUSE_MOVE
MOUSE_BUTTON_PRESS
WINDOW_CLOSE
```

## State

Describes the current condition.

```text
Key is down
Mouse button is down
Cursor is at X/Y
```

Example:

```text
             Event
               │
         "Space pressed"
               │
               ▼
          Input State
               │
               ▼
        Space = DOWN
```

---

# Event Processing

A typical application loop can look like:

```c
while (!wr_window_should_close(window))
{
    wr_poll_events();

    update();

    render();
}
```

The event processing flow is:

```text
wr_poll_events()
       │
       ▼
Backend
       │
       ▼
Native Events
       │
       ▼
WrEvent
       │
       ▼
Event Queue
       │
       ▼
Application
```

---

# Callbacks vs Event Queue

Walrus may support callbacks for convenience.

For example:

```c
wr_set_key_callback(
    window,
    key_callback
);
```

However, callbacks should be built on top of the event system rather than replacing it.

Conceptually:

```text
                 WrEvent
                    │
            ┌───────┴────────┐
            │                │
        Event Queue       Callback
            │                │
            └───────┬────────┘
                    │
               Application
```

This keeps the underlying event architecture consistent.

---

# UI Input

The UI system can consume events from the same input system.

For example:

```text
Mouse Click
    │
    ▼
WrEvent
    │
    ▼
Input System
    │
    ▼
UI Hit Testing
    │
    ▼
Button
    │
    ▼
Button Callback
```

The UI should not need to know whether the click originated from Wayland or X11.

---

# Input Coordinate System

Walrus should define a consistent coordinate system.

A common 2D UI coordinate system is:

```text
(0,0) ───────────────► X
  │
  │
  │
  │
  ▼
  Y
```

The top-left corner represents `(0, 0)`.

This is convenient for UI layout because coordinates increase toward the right and downward.

The backend converts native coordinates into the Walrus coordinate system if necessary.

---

# Focus

Input focus belongs to the window and UI systems.

Conceptually:

```text
Keyboard
    │
    ▼
Focused Window
    │
    ▼
Focused UI Element
```

For example:

```text
Window
  │
  └── Text Input
         │
         └── receives keyboard events
```

The event system should provide enough information for the UI layer to determine which element currently owns input focus.

---

# Event Propagation

UI events can eventually propagate through the widget hierarchy.

For example:

```text
Window
  │
  ▼
Root Widget
  │
  ▼
Panel
  │
  ▼
Button
```

A mouse click may be processed as:

```text
Mouse Event
     │
     ▼
Hit Test
     │
     ▼
Button
     │
     ▼
Handle Event
```

Future event propagation may support:

```text
capture
target
bubble
```

similar to other UI systems.

---

# Input for Games

Walrus is intended to also be usable by game engines.

Therefore, the input system should not be limited to traditional GUI input.

It should eventually support:

```text
Keyboard
Mouse
Touch
Gamepad
Controller
Raw input
```

The architecture should keep the device layer separate from the UI layer.

```text
             Input System
                  │
       ┌──────────┼──────────┐
       │          │          │
   Keyboard     Mouse      Gamepad
       │          │          │
       └──────────┼──────────┘
                  │
          Application / UI
```

A game can consume input directly without going through the UI system.

---

# Backend Independence

Wayland and X11 should produce the same logical events.

For example:

```text
Wayland:
wl_keyboard.key
       │
       ▼
WR_EVENT_KEY_PRESS
```

and:

```text
X11:
KeyPress
       │
       ▼
WR_EVENT_KEY_PRESS
```

The application sees only:

```text
WR_EVENT_KEY_PRESS
```

This is the core purpose of the input abstraction.

---

# Design Rules

The input architecture follows these rules:

1. Platform backends handle native input protocols.
2. Native events are converted into `WrEvent`.
3. The core input system maintains device state.
4. UI widgets consume generic events.
5. Applications can consume input without using the UI system.
6. Platform-specific input types must not leak into the public API.
7. Keyboard events and text input are separate concepts.
8. Events and current input state are separate concepts.

The key architectural rule is:

> **Backends report what happened; the input system represents it; the application and UI decide what it means.**
