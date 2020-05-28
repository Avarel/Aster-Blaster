#include "color.h"
#include <math.h>

const rgb_color_t COLOR_BLACK = { .r = 0, .g = 0, .b = 0 };

rgb_color_t rgb(float r, float g, float b) {
    return (rgb_color_t){.r = r, .g = g, .b = b};
}

rgb_color_t hsv(float h, float s, float v) {
    float c = s * v;
    float hh = h / 60;
    float x = c * (1 - fabs(fmod(hh, 2) - 1));
    float m = v - c;

    float r;
    float g;
    float b;


    if (0 <= hh && hh <= 1) {
        r = c, g = x, b = 0;
    }
    else if (1 < hh && hh <= 2) {
        r = x, g = c, b = 0;
    }
    else if (2 < hh && hh <= 3) {
        r = 0, g = c, b = x;
    }
    else if (3 < hh && hh <= 4) {
        r = 0, g = x, b = c;
    }
    else if (4 < hh && hh <= 5) {
        r = x, g = 0, b = c;
    }
    else if (5 < hh && hh <= 6) {
        r = c, g = 0, b = x;
    }
    else {
        r =0, g = 0, b = 0;
    }
    r += m;
    g += m;
    b += m;
    return rgb(r, g, b);
}
