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

// SDL settings
const vector_t SDL_MIN = {0, 0}; // TODO: check we use all these settings! Implement or remove if not
const vector_t SDL_MAX = {1200, 800}; // linked to by INIT_PLAYER_POS

typedef struct keypress_aux {
    scene_t *scene;
    body_t *player;
    size_t key_down;
    size_t last_key_down;
} keypress_aux_t;

/**********
 * KEYBOARD INPUT
 **********/
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
void on_key(char key, key_event_type_t type, double held_time, keypress_aux_t *keypress_aux) {
    if (type == KEY_PRESSED) {
        switch (key) {
        case LEFT_ARROW:
            // body_set_velocity(keypress_aux->player, vec_x(-PLAYER_SPEED));
            break;
        case RIGHT_ARROW:
            // body_set_velocity(keypress_aux->player, vec_x(PLAYER_SPEED));
            break;
        }
        set_key_down(key, true, keypress_aux);
    } else if (type == KEY_RELEASED && key == keypress_aux->last_key_down) {
        set_key_down(key, false, keypress_aux);
        if (key == RIGHT_ARROW || key == LEFT_ARROW) {
            // body_set_velocity(keypress_aux->player, VEC_ZERO);
        }
    }
}

int main() {
    sdl_init(SDL_MIN, SDL_MAX);
    init_random();

    sdl_on_key((key_handler_t)on_key);

    scene_t *scene = scene_init();

    keypress_aux_t *keypress_aux = malloc(sizeof(keypress_aux_t));
    keypress_aux->scene = scene;
    // keypress_aux->player = player;
    keypress_aux->key_down = 0;
    keypress_aux->last_key_down = 0;

    size_t frame = 0;

    while (!sdl_is_done(keypress_aux)) {
        double dt = time_since_last_tick();

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        frame++;
    }

    printf("Frames rendered: %zu\n", frame);
}