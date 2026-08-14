#ifndef WR_ELEMENT_H
#define WR_ELEMENT_H

typedef enum WrElementType {
  WR_ELEMENT_TYPE_WIDGET,
  WR_ELEMENT_TYPE_TEXT,
  WR_ELEMENT_IMAGE,
} WrElementType;

typedef struct WrElement WrElement;

struct WrElement {
  WrElementType type;

  void* data;
};


#endif
