/********************
 * IMPORTS
 ********************/
// C lib
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
// high level
#include "scene.h"
#include "sdl_wrapper.h"
// mid level
#include "forces.h"
#include "body.h"
#include "polygon.h"
#include "collision.h"
// low level
#include "list.h"
#include "vector.h"
#include "color.h"
#include "utils.h"
#include "math.h"

/********************
 * SETTINGS
 ********************/
// SDL settings
#define SDL_MIN ((vector_t) {.x =    0, .y =   0})
#define SDL_MAX ((vector_t) {.x = 1200, .y = 800})
/**
 * Font designed by JoannaVu
 * Licensed for non-commercial use
 * Downloaded from fontspace.com/andromeda-font-f31762
 */
#define FONT_TTF_PATH "../andromeda.ttf"
//Asteroid settings
const int ASTEROID_SPEED = 100;
const int ASTEROID_RADIUS_MIN = 30;
const int ASTEROID_RADIUS_MAX = 70;
const rgb_color_t ASTEROID_COLOR = {0.8, 0.8, 0.8};


/********************
 * STRUCTS & ENUMS
 ********************/
typedef enum window {
    MENU,
    GAME
} window_e;

typedef enum etype {
    BULLET,
    PLAYER,
    ASTEROID
} etype_t;

typedef struct aster_aux {
    etype_t type;
} aster_aux_t;

typedef struct keypress_aux {
    scene_t *scene;
    body_t *player;
    size_t key_down;
    size_t last_key_down;
    window_e window;
} keypress_aux_t;

/********************
 * KEYBOARD INPUT
 ********************/
/**
 * Updates the keypress_aux key values
 * @param key the key pressed or released
 * @param key_is_down whether the key in question is currently down
 * @param keypress_aux the aux to modify
 */
void set_key_down(char key, bool key_is_down, keypress_aux_t *keypress_aux) {
    if (key == LEFT_ARROW || key == RIGHT_ARROW) {
        keypress_aux->key_down = key_is_down ? key : 0;
        keypress_aux->last_key_down = key;
    }
}

/**
 * Handles the player controls. Allows for lateral player movement and player bullet firing.
 * @param key the key pressed or released
 * @param type whether the key was pressed or released
 * @param held_time the time the key has been held
 */
void on_key_menu(char key, key_event_type_t type, double held_time, keypress_aux_t *keypress_aux) {
    if (type == KEY_PRESSED) {
        switch (key) {
        case SPACE_BAR:
            if (keypress_aux->window == MENU) {
                keypress_aux->window = GAME;
            }
            break;
        }
        set_key_down(key, true, keypress_aux);
    } else if (type == KEY_RELEASED && key == keypress_aux->last_key_down) {
        set_key_down(key, false, keypress_aux);
    }
}

/********************
 * ASTEROID GENERATION
 ********************/
void spawn_asteroid(scene_t *scene, list_t *bullets, body_t *player){
    //random later
    num_sides = 5;
    ast_radius = ASTEROID_RADIUS_MIN;
    ast_center = vec(0,0);
    ast_velocity = vec(ASTEROID_SPEED, -ASTEROID_SPEED);
    
    list_t *aster_shape = polygon_reg_ngon()
}



/********************
 * GAME LOGIC
 ********************/
int main() {
    sdl_init(SDL_MIN, SDL_MAX);
    init_random();

    scene_t *scene = scene_init();


    sdl_on_key((key_handler_t)on_key_menu);
    keypress_aux_t *keypress_aux = malloc(sizeof(keypress_aux_t));
    keypress_aux->scene = scene;
    keypress_aux->window = MENU;

    size_t frame = 0;
    size_t debug_print_rate = 200;

    while (!sdl_is_done(keypress_aux)) {
        double dt = time_since_last_tick();

        if (frame % debug_print_rate == 0) {
            printf("Window: %s\n", keypress_aux->window == MENU ? "menu" : "game");
        }

        switch (keypress_aux->window) {
            case MENU:
                break;
            case GAME:
                break;
        }

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        frame++;
    }

    printf("Frames rendered: %zu\n", frame);

    // free mallocs
    free(keypress_aux);
}
