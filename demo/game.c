/********************
 * IMPORTS
 ********************/
// C lib
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// high level
#include "scene.h"
#include "sdl_wrapper.h"
// mid level
#include "body.h"
#include "collision.h"
#include "forces.h"
#include "polygon.h"
// low level
#include "color.h"
#include "list.h"
#include "math.h"
#include "utils.h"
#include "vector.h"

/********************
 * SETTINGS
 ********************/
// SDL settings
#define SDL_MIN ((vector_t){.x = 0, .y = 0})
#define SDL_MAX ((vector_t){.x = 1200, .y = 800})
/**
 * Font designed by JoannaVu
 * Licensed for non-commercial use
 * Downloaded from fontspace.com/andromeda-font-f31762
 */
char FONT_PATH_ASTER_BLASTER[] = "./andromeda.ttf\0";
// Menu settings
// note: text_box's size element currently isn't used by renderer
char MENU_TITLE_TEXT[] = "Aster Blaster\0";
#define MENU_TITLE_FONT_SIZE 40
//#define MENU_TITLE_SIZE ((vector_t) {.x = SDL_MAX.x / 4, .y = SDL_MAX.y / 10})
#define MENU_TITLE_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.75 * SDL_MAX.y})

char MENU_GAME_START_TEXT[] = "Press space to begin!\0";
#define MENU_GAME_START_FONT_SIZE 20
//#define MENU_GAME_START_SIZE ((vector_t) {.x = SDL_MAX.x / 4, .y = SDL_MAX.y / 15})
#define MENU_GAME_START_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.6 * SDL_MAX.y})

// Asteroid settings
#define ASTEROID_SPEED 100
#define ASTEROID_RADIUS_MIN 30
#define ASTEROID_RADIUS_MAX 70
#define ASTEROID_COLOR ((rgb_color_t){0.8, 0.8, 0.8})

const size_t debug_print_rate = 200;

/********************
 * STRUCTS & ENUMS
 ********************/
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
} aster_aux_t;

/********************
 * KEYBOARD INPUT
 ********************/

typedef struct menu_keypress_aux {
    scene_t *scene;
    size_t key_down;
    window_type_e window;
} menu_keypress_aux_t;

/**
 * Handles the player controls. Allows for lateral player movement and player bullet firing.
 * @param key the key pressed or released
 * @param type whether the key was pressed or released
 * @param held_time the time the key has been held
 */
void on_key_menu(char key, key_event_type_t type, double held_time, menu_keypress_aux_t *keypress_aux) {
    if (type == KEY_PRESSED) {
        switch (key) {
        case SPACE_BAR:
            if (keypress_aux->window == MENU) {
                // assumes specific text_box state
                scene_remove_text_box(keypress_aux->scene, 1);
                scene_remove_text_box(keypress_aux->scene, 0);
                keypress_aux->window = GAME;
            }
            break;
        }
        keypress_aux->key_down = set_nth_bit(keypress_aux->key_down, key);
    } else if (type == KEY_RELEASED) {
        keypress_aux->key_down = clear_nth_bit(keypress_aux->key_down, key);
    }
}

typedef struct game_keypress_aux {
    scene_t *scene;
    body_t *player;
    size_t key_down;
    window_type_e window;
} game_keypress_aux_t;

void on_key_game(char key, key_event_type_t type, double held_time, game_keypress_aux_t *keypress_aux) {
    if (type == KEY_PRESSED) {
        keypress_aux->key_down = set_nth_bit(keypress_aux->key_down, key);
    } else if (type == KEY_RELEASED) {
        keypress_aux->key_down = clear_nth_bit(keypress_aux->key_down, key);
    }
}

/********************
 * ASTEROID GENERATION
 ********************/
void spawn_asteroid(scene_t *scene, list_t *bullets, body_t *player) {
    //random later
    /* num_sides = 5;
    ast_radius = ASTEROID_RADIUS_MIN;
    ast_center = vec(0,0);
    ast_velocity = vec(ASTEROID_SPEED, -ASTEROID_SPEED);
    
    list_t *aster_shape = polygon_reg_ngon() */
}

/********************
 * GAME LOGIC
 ********************/
void menu_loop();
void game_loop();
void control_loop(); // TODO: later

int main() {
    sdl_init(SDL_MIN, SDL_MAX);
    sdl_set_font(&FONT_PATH_ASTER_BLASTER[0]);
    init_random();

    menu_loop();
}

void printBits(unsigned int num)
{
   for(int bit=0;bit<(sizeof(unsigned int) * 2); bit++)
   {
      printf("%i ", num & 0x01);
      num = num >> 1;
   }
   printf("\n");
}

void menu_loop() {
    scene_t *scene = scene_init();

    sdl_on_key((key_handler_t)on_key_menu);

    menu_keypress_aux_t *menu_keypress_aux = malloc(sizeof(menu_keypress_aux_t));
    menu_keypress_aux->scene = scene;
    menu_keypress_aux->key_down = 0;
    menu_keypress_aux->window = MENU;

    text_box_t *menu_title_text_box = text_box_init(&MENU_TITLE_TEXT[0], MENU_TITLE_FONT_SIZE, MENU_TITLE_ORIGIN);
    scene_add_text_box(scene, menu_title_text_box);
    text_box_t *menu_game_start_text_box = text_box_init(&MENU_GAME_START_TEXT[0], MENU_GAME_START_FONT_SIZE, MENU_GAME_START_ORIGIN);
    scene_add_text_box(scene, menu_game_start_text_box);

    size_t frame = 0;

    bool to_game = false;
    while (!sdl_is_done(menu_keypress_aux)) {
        double dt = time_since_last_tick();

        if (frame % debug_print_rate == 0) {
            // printf("window: %s\n", keypress_aux->window == MENU ? "menu" : "game");
        }

        if (menu_keypress_aux->window == GAME) {
            to_game = true;
            break;
        }

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        frame++;
    }

    printf("Frames rendered: %zu\n", frame);

    // free mallocs
    free(menu_keypress_aux);
    scene_free(scene);

    if (to_game) {
        game_loop();
    }
}

void game_loop() {
    scene_t *scene = scene_init();

    sdl_on_key((key_handler_t)on_key_game);

    body_t *body = body_init(polygon_star(vec(500, 500), 100, 50, 5), 50, COLOR_BLACK);
    scene_add_body(scene, body);

    game_keypress_aux_t *game_keypress_aux = malloc(sizeof(game_keypress_aux_t));
    game_keypress_aux->scene = scene;
    game_keypress_aux->player = body; // temporary
    game_keypress_aux->key_down = 0;
    game_keypress_aux->window = MENU;

    size_t frame = 0;
    
    while (!sdl_is_done(game_keypress_aux)) {
        double dt = time_since_last_tick();

        if (frame % debug_print_rate == 0) {
            // printf("window: %s\n", keypress_aux->window == MENU ? "menu" : "game");
        }

        printBits(game_keypress_aux->key_down);

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        frame++;
    }

    free(game_keypress_aux);
    scene_free(scene);
}