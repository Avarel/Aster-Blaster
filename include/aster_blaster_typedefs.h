#ifndef __ASTER_BLASTER_TYPEDEFS__
#define __ASTER_BLASTER_TYPEDEFS__

#include <stdlib.h>
#include <stdbool.h>
#include "scene.h"

typedef enum window_type {
    NONE = 0, // essentially NULL
    MENU = 1,
    CONTROLS = 2,
    GAME = 3,
} window_type_e;

typedef enum body_type {
    BULLET,
    PLAYER,
    ASTEROID
} body_type_e;

typedef struct aster_aux {
    body_type_e body_type;
    double health;
    body_t *health_bar;
    bool game_over;
} aster_aux_t;

typedef struct menu_keypress_aux {
    scene_t *scene;
    size_t key_down;
    window_type_e window;
} menu_keypress_aux_t;

#endif // #ifndef __ASTER_BLASTER_TYPEDEFS__
