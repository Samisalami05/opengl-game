#ifndef COLOR_H
#define COLOR_H

#include "math/vec3.h"

typedef vec3 Color;

#define COL_WHITE   (Color){1.0f, 1.0f, 1.0f}
#define COL_BLACK   (Color){0.0f, 0.0f, 0.0f}

#define COL_RED     (Color){1.0f, 0.0f, 0.0f}
#define COL_GREEN   (Color){0.0f, 1.0f, 0.0f}
#define COL_BLUE    (Color){0.0f, 0.0f, 1.0f}

#define COL_YELLOW  (Color){1.0f, 1.0f, 0.0f}
#define COL_CYAN    (Color){0.0f, 1.0f, 1.0f}
#define COL_MAGENTA (Color){1.0f, 0.0f, 1.0f}

#define COL_GRAY    (Color){0.5f, 0.5f, 0.5f}
#define COL_LIGHT_GRAY (Color){0.75f, 0.75f, 0.75f}
#define COL_DARK_GRAY  (Color){0.25f, 0.25f, 0.25f}

#define COL_ORANGE  (Color){1.0f, 0.5f, 0.0f}
#define COL_PURPLE  (Color){0.5f, 0.0f, 0.5f}
#define COL_PINK    (Color){1.0f, 0.75f, 0.8f}

#define COL_LIME    (Color){0.75f, 1.0f, 0.0f}
#define COL_TEAL    (Color){0.0f, 0.5f, 0.5f}
#define COL_NAVY    (Color){0.0f, 0.0f, 0.5f}

#endif
