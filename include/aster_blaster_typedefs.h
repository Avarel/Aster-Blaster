#ifndef __ASTER_BLASTER_TYPEDEFS__
#define __ASTER_BLASTER_TYPEDEFS__

#include "aster_blaster_imports.h"

typedef enum window_type {
    NONE = 0, // essentially NULL
    MENU = 1,
    CONTROLS = 2,
    GAME = 3,
} window_type_e;

typedef enum body_type {
    BULLET,
    PLAYER,
    ASTEROID,
    ENEMY_SAW,
    ENEMY_SHOOTER,
    BLACK_HOLE
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

typedef struct game_bounds {
    body_t *left;
    body_t *right;
    body_t *top;
    body_t *bottom;
} game_bounds_t;

typedef struct game_keypress_aux {
    scene_t *scene;
    body_t *player;
    size_t key_down;
    window_type_e window;
} game_keypress_aux_t;

#endif // #ifndef __ASTER_BLASTER_TYPEDEFS__
