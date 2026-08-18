#ifndef WR_VERTEX_H
#define WR_VERTEX_H

typedef struct WrVertex {
  float x, y;
  float r, g, b, a;
  float u, v;
  float rect_x, rect_y, rect_w, rect_h;
  float radius;
  float shape_type;
} WrVertex;

#endif
