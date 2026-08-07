#ifndef WALRUS_NODE_H
#define WALRUS_NODE_H

typedef struct wr_node wr_node_t;

struct wr_node {
    wr_node_type_t type;

    wr_node_t *parent;
    wr_node_t *first_child;
    wr_node_t *next_sibling;

    wr_style_t style;
    wr_rect_t bounds;

    void *data;
}

#endif