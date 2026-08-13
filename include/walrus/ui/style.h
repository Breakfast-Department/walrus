#ifndef WR_STYLE_H
#define WR_STYLE_H

typedef struct WrColor {
  float r;
  float g;
  float b;
  float a;
} WrColor;


typedef enum WrGradientType {
  WR_GRADIENT_LINEAR,
  WR_GRADIENT_RADIAL
} WrGradientType;

typedef struct WrGradientStop {
  float position;
  WrColor color;
} WrGradientStop;

typedef struct WrGradientColor {
  WrGradientType type;

  float angle;

  WrGradientStop* stops;
  unsigned int stop_count;
} WrGradientColor;


typedef struct WrBackgroundImage {
  const char* source;

  float opacity;

  float position_x;
  float position_y;

  float scale_x;
  float scale_y;
} WrBackgroundImage;


typedef enum WrBackgroundType {
  WR_BACKGROUND_COLOR,
  WR_BACKGROUND_GRADIENT,
  WR_BACKGROUND_IMAGE
} WrBackgroundType;

typedef struct WrBackground {
  WrBackgroundType type;

  union {
    WrColor color;
    WrGradientColor gradient;
    WrBackgroundImage image;
  };
} WrBackground;


typedef struct WrSpacing {
  float top;
  float right;
  float bottom;
  float left;
} WrSpacing;


typedef struct WrLayoutStyle {
  float width;
  float height;

  WrSpacing margin;
  WrSpacing padding;
} WrLayoutStyle;


typedef struct WrFont {
  const char* family;

  float size;
  float weight;
  float style;
} WrFont;


typedef struct WrTextStyle {
  WrFont font;
  WrColor color;
} WrTextStyle;


typedef struct WrBorder {
  WrColor color;

  float width;
  float radius;
} WrBorder;


typedef struct WrShadow {
  float offset_x;
  float offset_y;

  float blur;

  WrColor color;
} WrShadow;


typedef struct WrStyle {
  WrLayoutStyle layout;

  WrBackground background;
  WrColor foreground;

  WrTextStyle text;

  WrBorder border;

  WrShadow shadow;

  float opacity;
} WrStyle;


typedef enum WrStyleState {
  WR_STATE_NORMAL,
  WR_STATE_HOVER,
  WR_STATE_ACTIVE,
  WR_STATE_FOCUSED,
  WR_STATE_DISABLED
} WrStyleState;


typedef struct WrStyleRule {
  WrStyleState state;

  WrStyle style;
} WrStyleRule;

#endif
