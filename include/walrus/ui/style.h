#ifndef WR_STYLE_H
#define WR_STYLE_H

typedef struct WrStyle {

} WrStyle;

typedef struct WrStyleRule {
  const char* selector;
  WrStyle style;
} WrStyleRule;

#endif
