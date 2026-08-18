#ifndef WR_INPUT_H
#define WR_INPUT_H

#include <walrus/ui/style.h>

typedef enum {
  WR_INPUT_EVENT_NONE = 0,
  WR_INPUT_EVENT_WINDOW_RESIZE,
  WR_INPUT_EVENT_MOUSE_MOVE,
  WR_INPUT_EVENT_MOUSE_BUTTON,
} WrInputEventType;

typedef struct {
  WrInputEventType type;
  union {
    struct { int width; int height; } resize;
    struct { int x; int y; int button; int pressed; } mouse;
  } data;
} WrInputEvent;

#endif
