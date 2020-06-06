#include "color.h"


#include "text_box.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <assert.h>
#include <math.h>
#include "sdl_wrapper.h"
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>

const rgb_color_t COLOR_BLACK = {.r = 0, .g = 0, .b = 0};
const rgb_color_t COLOR_WHITE = {.r = 1, .g = 1, .b = 1};

texture_t texture_color(rgb_color_t color) {
    return (texture_t){.type = COLOR, .data = (texture_data_t){.color = color}};
}

texture_t texture_image(SDL_Surface *surface) {
    // return texture_color(COLOR_BLACK);
    assert(surface != NULL);
    return (texture_t){.type = SURFACE, .data = (texture_data_t){.surface = surface}};
}

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
    } else if (1 < hh && hh <= 2) {
        r = x, g = c, b = 0;
    } else if (2 < hh && hh <= 3) {
        r = 0, g = c, b = x;
    } else if (3 < hh && hh <= 4) {
        r = 0, g = x, b = c;
    } else if (4 < hh && hh <= 5) {
        r = x, g = 0, b = c;
    } else if (5 < hh && hh <= 6) {
        r = c, g = 0, b = x;
    } else {
        r = 0, g = 0, b = 0;
    }
    r += m;
    g += m;
    b += m;
    return rgb(r, g, b);
}
