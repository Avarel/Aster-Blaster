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
#include "text_box.h"
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
char MENU_TITLE_TEXT[] = "Aster Blaster\0";
#define MENU_TITLE_FONT_SIZE 40
#define MENU_TITLE_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.75 * SDL_MAX.y})
#define MENU_TITLE_JUSTIFICATION CENTER

char MENU_GAME_START_TEXT[] = "Press space to begin!\0";
#define MENU_GAME_START_FONT_SIZE 20
#define MENU_GAME_START_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.6 * SDL_MAX.y})
#define MENU_GAME_START_JUSTIFICATION CENTER

// Player settings
#define PLAYER_INIT_POS ((vector_t){.x = SDL_MAX.x / 2, .y = 0.1 * SDL_MAX.y})
#define PLAYER_RADIUS 50
#define PLAYER_SIDES 15
#define PLAYER_SECTOR_SIDES 12
#define PLAYER_ANGLE (M_PI / 2)
#define PLAYER_MASS 100
#define PLAYER_COLOR ((rgb_color_t){0.8, 0.3, 0.5})

// Asteroid settings
#define ASTEROID_SPEED 100
#define ASTEROID_RADIUS_MIN 30
#define ASTEROID_RADIUS_MAX 70
#define ASTEROID_COLOR ((rgb_color_t){0.8, 0.8, 0.8})

// Background settings
#define NUM_STARS 150
#define STAR_RADIUS_MIN 3
#define STAR_RADIUS_MAX 8
#define STAR_POINTS_MIN 7
#define STAR_POINTS_MAX 10
#define STAR_VELOCITY_1 ((vector_t){.x = 0, .y = -0.1 * SDL_MAX.y})
#define STAR_VELOCITY_2 ((vector_t){.x = 0, .y = -0.15 * SDL_MAX.y})
#define STAR_COLOR ((rgb_color_t){1.0, 1.0, 1.0})

#define DEBUG_PRINT_RATE 200

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


typedef struct menu_keypress_aux {
    scene_t *scene;
    size_t key_down;
    window_type_e window;
} menu_keypress_aux_t;

/********************
 * KEYBOARD INPUT
 ********************/
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
 * Background Stuff
 ********************/

 /**
  * Handles looping of the background sars. When the yhit the bound below the
  * bottom of the screen they teleport back to the top.
  */
 void create_star_collision_force(body_t *star, body_t *bound, vector_t axis, void *aux) {
     body_set_centroid(star, vec_add(body_get_centroid(star), vec(0, SDL_MAX.y + 8 * STAR_RADIUS_MAX)));
 }

 void create_star_collision(scene_t *scene, body_t *star, body_t *bound) {
     create_collision(scene, star, bound, create_star_collision_force, NULL, free);
 }

 /**
  * Creates all of the background stars and adds them to the scene. It is
  * important that this is called before any other bodies are added to the
  * scene so the stars go in the background.
  */
void create_background_stars(scene_t *scene, body_t *bound) {
    for(int i = 0; i < NUM_STARS; i++) {
        double r = drand_range(STAR_RADIUS_MIN, STAR_RADIUS_MAX);
        size_t degree = (size_t) irand_range(STAR_POINTS_MIN, STAR_POINTS_MAX);
        vector_t center = rand_vec(SDL_MIN, SDL_MAX);
        list_t *shape = polygon_star(center, r, r/2, degree);
        body_t *star = body_init(shape, 0, STAR_COLOR);

        // Gives the star one of two velocities to create the illusion of
        // parallax.
        int which_velocity = irand_range(1,2);
        switch (which_velocity) {
            case 1:
                body_set_velocity(star, STAR_VELOCITY_1);
                break;
            case 2:
                body_set_velocity(star, STAR_VELOCITY_2);
                break;
        }
        create_star_collision(scene, star, bound);

        scene_add_body(scene, star);
    }
}

/********************
 * ASTEROID GENERATION
 ********************/
 void create_destructive_collision_force_single(body_t *body1, body_t *body_immortal, vector_t axis, void *aux) {
     body_remove(body1);
 }

 void create_destructive_collision_single(scene_t *scene, body_t *body1, body_t *body_immortal) {
     create_collision(scene, body1, body_immortal, create_destructive_collision_force_single, NULL, NULL);
 }

 void create_special_collision_force(body_t *ast, body_t *player, vector_t axis, void *aux) {
     body_remove(ast);
     // Bruno, do what you want to here
 }

 void create_special_collision(scene_t *scene, body_t *ast, body_t *player) {
     create_collision(scene, ast, player, create_destructive_collision_force_single, NULL, NULL);
 }


void spawn_asteroid(
    scene_t *scene,
    body_t *left_bound,
    body_t *right_bound,
    body_t *top_bound,
    body_t *bottom_bound) {
    //random later
    size_t num_sides = 5;
    double ast_radius = ASTEROID_RADIUS_MIN;
    vector_t ast_center = vec(0,SDL_MAX.y);
    vector_t ast_velocity = vec(ASTEROID_SPEED, -ASTEROID_SPEED);

    aster_aux_t *asteroid_aux = malloc(sizeof(aster_aux_t));
    asteroid_aux->body_type = ASTEROID;

    list_t *aster_shape = polygon_reg_ngon(ast_center, ast_radius, num_sides);
    body_t *asteroid = body_init_with_info(aster_shape, 0, ASTEROID_COLOR, asteroid_aux, free);
    body_set_velocity(asteroid, ast_velocity);

    scene_add_body(scene, asteroid);
    for(size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if(other_aux != NULL){
            if (other_aux->body_type == BULLET) {
                create_destructive_collision(scene, asteroid, other_body);
            }
            else if(other_aux->body_type == PLAYER){
                create_special_collision(scene, asteroid, other_body);
            }
        }
    }
    create_destructive_collision_single(scene, asteroid, top_bound);
    create_destructive_collision_single(scene, asteroid, bottom_bound);
    create_destructive_collision_single(scene, asteroid, right_bound);
    create_destructive_collision_single(scene, asteroid, left_bound);
}

/********************
 * PLAYER
 ********************/
body_t *body_init_player() {
    list_t *player_shape = polygon_ngon_sector(PLAYER_INIT_POS, PLAYER_RADIUS, PLAYER_SIDES, PLAYER_SECTOR_SIDES, PLAYER_ANGLE);
    body_t *player = body_init(player_shape, PLAYER_MASS, PLAYER_COLOR);
    return player;
}

/********************
 * GAME LOOPS
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

void print_bits(unsigned int num) {
   for(int bit=0 ;bit < (sizeof(unsigned int) * 2); bit++) {
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

    text_box_t *menu_title_text_box = text_box_init(&MENU_TITLE_TEXT[0], MENU_TITLE_FONT_SIZE, MENU_TITLE_ORIGIN, MENU_TITLE_JUSTIFICATION);
    scene_add_text_box(scene, menu_title_text_box);
    text_box_t *menu_game_start_text_box = text_box_init(&MENU_GAME_START_TEXT[0], MENU_GAME_START_FONT_SIZE, MENU_GAME_START_ORIGIN, MENU_GAME_START_JUSTIFICATION);
    scene_add_text_box(scene, menu_game_start_text_box);

    size_t frame = 0;

    bool to_game = false;
    while (!sdl_is_done(menu_keypress_aux)) {
        double dt = time_since_last_tick();

        if (frame % DEBUG_PRINT_RATE == 0) {
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

    // using ASTEROID_RADIUS for bounds because it's maximum size
    body_t *top_bound = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MAX.y + ASTEROID_RADIUS_MAX), SDL_MAX.x, ASTEROID_RADIUS_MAX), INFINITY, COLOR_BLACK);
    body_t *bottom_bound = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MIN.y - 3 * ASTEROID_RADIUS_MAX), SDL_MAX.x, ASTEROID_RADIUS_MAX), INFINITY, COLOR_BLACK);
    body_t *left_bound = body_init(polygon_rect(vec(SDL_MIN.x - 3 * ASTEROID_RADIUS_MAX, SDL_MIN.y), ASTEROID_RADIUS_MAX, SDL_MAX.y), INFINITY, COLOR_BLACK);
    body_t *right_bound = body_init(polygon_rect(vec(SDL_MAX.x + ASTEROID_RADIUS_MAX, SDL_MIN.y), ASTEROID_RADIUS_MAX, SDL_MAX.y), INFINITY, COLOR_BLACK);

    scene_add_body(scene, left_bound);
    scene_add_body(scene, right_bound);
    scene_add_body(scene, top_bound);
    scene_add_body(scene, bottom_bound);

    // when stars collide with this offscreen they'll teleport to just off the
    // top of the scene so it loops.
    body_t *star_bound = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MIN.y - 6 * STAR_RADIUS_MAX), SDL_MAX.x, 2 * STAR_RADIUS_MAX), INFINITY, COLOR_BLACK);
    create_background_stars(scene, star_bound);

    body_t *body = body_init(polygon_star(vec(500, 500), 100, 50, 5), 50, COLOR_WHITE);
    scene_add_body(scene, body);

    spawn_asteroid(scene, left_bound, right_bound, top_bound, bottom_bound);

    game_keypress_aux_t *game_keypress_aux = malloc(sizeof(game_keypress_aux_t));
    game_keypress_aux->scene = scene;
    game_keypress_aux->player = body; // temporary
    game_keypress_aux->key_down = 0;
    game_keypress_aux->window = MENU;

    body_t *player = body_init_player();
    scene_add_body(scene, player);

    size_t frame = 0;

    while (!sdl_is_done(game_keypress_aux)) {
        double dt = time_since_last_tick();

        if (frame % DEBUG_PRINT_RATE == 0) {
            // printf("window: %s\n", keypress_aux->window == MENU ? "menu" : "game");
            // print_bits(game_keypress_aux->key_down);
        }

        

        scene_tick(scene, dt);
        sdl_render_scene_black(scene);
        frame++;
    }

    free(game_keypress_aux);
    scene_free(scene);
}
