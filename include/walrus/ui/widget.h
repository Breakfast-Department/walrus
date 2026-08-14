#ifndef WR_WIDGET_H
#define WR_WIDGET_H

#include "walrus/ui/element.h"
#include "walrus/ui/style.h"

/* Forward declare render surface type so widget header doesn't need renderer.h */
typedef struct WrRenderSurface WrRenderSurface;

typedef struct WrWidget WrWidget;

struct WrWidget {
  WrStyle style;

  WrStyleRule* rules;
  unsigned int rule_count;

  WrElement* parent;

  WrElement** children;
  unsigned int child_count;

  void (*render)(WrWidget* widget, WrRenderSurface* surface);

  void (*add_rule)(
    WrWidget* widget,
    const WrStyleRule* rule
  );

  void (*add_child)(
    WrWidget* widget,
    WrElement* child
  );

  void (*remove_child)(
    WrWidget* widget,
    WrElement* child
  );

  void (*set_parent)(
    WrWidget* widget,
    WrElement* parent
  );
};

/* Create/destroy helpers */
WrWidget* wr_create_widget(const char* selector);
void wr_widget_destroy(WrWidget* widget);


#endif
