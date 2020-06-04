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
#include "text_box.h"
// low level
#include "color.h"
#include "list.h"
#include "math.h"
#include "utils.h"
#include "vector.h"
// demo modules
#include "aster_blaster_settings.h"
#include "aster_blaster_typedefs.h"


// TODO: split into more demo modules (e.g. aster_blaster_body_inits)?
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
 * BACKGROUND STARS
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
    for (int i = 0; i < NUM_STARS; i++) {
        double r = drand_range(STAR_RADIUS_MIN, STAR_RADIUS_MAX);
        size_t degree = (size_t)irand_range(STAR_POINTS_MIN, STAR_POINTS_MAX);
        vector_t center = rand_vec(SDL_MIN, SDL_MAX);
        list_t *shape = polygon_reg_ngon(center, r, degree);
        // list_t *shape = polygon_star(center, r, r / 2, degree);
        body_t *star = body_init(shape, 0, STAR_COLOR);

        // Gives the star one of three velocities to create the illusion of
        // parallax.
        int which_velocity = irand_range(1, 3);
        switch (which_velocity) {
        case 1:
            body_set_velocity(star, STAR_VELOCITY_1);
            break;
        case 2:
            body_set_velocity(star, STAR_VELOCITY_2);
            break;
        case 3:
            body_set_velocity(star, STAR_VELOCITY_3);
            break;
        }
        create_star_collision(scene, star, bound);

        scene_add_body(scene, star);
    }
}

/********************
 * ASTEROID GENERATION
 ********************/
void create_health_collision(body_t *body_to_remove, body_t *body_immortal, vector_t axis, void *aux) {
    aster_aux_t *aster_aux = body_get_info(body_immortal);

    aster_aux->health = fmax(0, aster_aux->health - body_get_mass(body_to_remove) * DAMAGE_PER_MASS);
    // relies on two right points being idx 1 and 2
    list_t *health_bar_shape = body_get_shape(aster_aux->health_bar);
    vector_t *bottom_right_point = list_get(health_bar_shape, 1);
    vector_t *top_right_point = list_get(health_bar_shape, 2);
    *bottom_right_point = vec(HEALTH_BAR_POS.x + HEALTH_BAR_W * (aster_aux->health / HEALTH_TOTAL), HEALTH_BAR_POS.y);
    *top_right_point = vec(HEALTH_BAR_POS.x + HEALTH_BAR_W * (aster_aux->health / HEALTH_TOTAL), HEALTH_BAR_POS.y + HEALTH_BAR_H);

    if (is_close(aster_aux->health, 0)) {
        aster_aux->game_over = true;
    }
    
    body_remove(body_to_remove);
}

void create_destructive_collision_force_single(body_t *body1, body_t *body_immortal, vector_t axis, void *aux) {
    // printf("OUCH!\n");
    body_remove(body1);
}

void create_destructive_collision_single(scene_t *scene, body_t *body1, body_t *body_immortal) {
    create_collision(scene, body1, body_immortal, create_destructive_collision_force_single, NULL, NULL);
}

void create_special_collision_force(body_t *ast, body_t *player, vector_t axis, void *aux) {
    body_remove(ast);
}

void create_aster_player_collision(scene_t *scene, body_t *ast, body_t *player) {
    create_collision(scene, ast, player, create_health_collision, NULL, NULL);
    create_physics_collision(scene, 1.0, player, ast);
}

void create_aster_bullet_collision(scene_t *scene, body_t *ast, body_t *bullet) {
    // style 2: physics (maybe even reduce mass!)
    // create_destructive_collision_single(scene, bullet, ast);
    // create_physics_collision(scene, 1.0, ast, bullet);

    // style 1: simple
    create_destructive_collision(scene, ast, bullet);
}

void spawn_asteroid(
    scene_t *scene,
    body_t *left_bound,
    body_t *right_bound,
    body_t *top_bound,
    body_t *bottom_bound) {
    // TODO: random later
    size_t num_sides = irand_range(ASTEROID_SIDES_MIN, ASTEROID_SIDES_MAX);
    double ast_radius = drand_range(ASTEROID_RADIUS_MIN, ASTEROID_RADIUS_MAX);
    double ast_x = drand_range(SDL_MIN.x, SDL_MAX.x);
    vector_t ast_center = vec(ast_x, SDL_MAX.y + ast_radius);

    //if the asteroid spawns at the left of the screen,x velocity should be
    //positive, so theta between 3*pi/2 and 2*pi
    //otherwise, theta between pi and 3*pi/2, so x velocity is negative
    double theta = 0;
    double midpoint = (SDL_MAX.x - SDL_MIN.x) / 2;
    if (ast_x < midpoint) {
        theta = drand_range(3 * M_PI / 2, 2 * M_PI);
    } else {
        theta = drand_range(M_PI, 3 * M_PI / 2);
    }
    vector_t ast_velocity = vec(ASTEROID_SPEED * cos(theta), ASTEROID_SPEED * sin(theta));

    aster_aux_t *asteroid_aux = malloc(sizeof(aster_aux_t));
    asteroid_aux->body_type = ASTEROID;

    list_t *aster_shape = polygon_reg_ngon(ast_center, ast_radius, num_sides);
    body_t *asteroid = body_init_with_info(aster_shape, ASTEROID_MASS, ASTEROID_COLOR, asteroid_aux, free);
    body_set_velocity(asteroid, ast_velocity);

    scene_add_body(scene, asteroid);
    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_aster_bullet_collision(scene, asteroid, other_body);
            } else if (other_aux->body_type == PLAYER) {
                create_aster_player_collision(scene, asteroid, other_body);
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
body_t *body_init_player(body_t *health_bar) {
    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = PLAYER;
    aster_aux->health = HEALTH_TOTAL;
    aster_aux->health_bar = health_bar;
    aster_aux->game_over = false;

    list_t *player_shape = polygon_ngon_sector(PLAYER_INIT_POS, PLAYER_RADIUS, PLAYER_SIDES, PLAYER_SECTOR_SIDES, PLAYER_ANGLE);
    body_t *player = body_init_with_info(player_shape, PLAYER_MASS, PLAYER_COLOR, aster_aux, free);
    return player;
}

/********************
 * BULLETS
 ********************/

body_t *body_init_bullet(body_t *player) {
    aster_aux_t *aux = malloc(sizeof(aster_aux_t));
    aux->body_type = BULLET;
    list_t *bullet_shape = polygon_reg_ngon(vec_add(body_get_centroid(player), vec_y(PLAYER_RADIUS)), BULLET_RADIUS, 3);
    body_t *bullet = body_init_with_info(bullet_shape, BULLET_MASS, BULLET_COLOR, aux, free);
    body_set_velocity(bullet, BULLET_VELOCITY);
    return bullet;
}

void spawn_bullet(scene_t *scene, body_t *player, body_t *bound) {
    body_t *bullet = body_init_bullet(player);
    create_destructive_collision_single(scene, bullet, bound);
    scene_add_body(scene, bullet);

    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == ASTEROID) {
                create_aster_bullet_collision(scene, other_body, bullet);
            }
        }
    }
}

/********************
 * BODY INITS
 ********************/
body_t *body_health_bar_background_init() {
    list_t *shape = polygon_rect(HEALTH_BAR_BACKGROUND_POS, HEALTH_BAR_BACKGROUND_W, HEALTH_BAR_BACKGROUND_H);
    body_t *health_bar_background = body_init(shape, 0, HEALTH_BAR_BACKGROUND_COLOR);
    return health_bar_background;
}

body_t *body_health_bar_init() {
    list_t *shape = polygon_rect(HEALTH_BAR_POS, HEALTH_BAR_W, HEALTH_BAR_H);
    body_t *health_bar_background = body_init(shape, 0, HEALTH_BAR_COLOR);
    return health_bar_background;
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
    for (int bit = 0; bit < (sizeof(unsigned int) * 2); bit++) {
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
            // assumes specific text_box state
            //scene_remove_text_box(scene, 1);
            //scene_remove_text_box(scene, 0);
            to_game = true;
            break;
        }

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        frame++;
    }

    //printf("Frames rendered: %zu\n", frame);

    // free mallocs
    free(menu_keypress_aux);
    scene_free(scene);

    if (to_game) {
        game_loop();
    }
}

void velocity_handle(
    body_t *body,
    size_t key_down,
    body_t *left_bound,
    body_t *right_bound,
    body_t *top_bound,
    body_t *bottom_bound) {
    // handle movement
    vector_t dv = VEC_ZERO;
    if (get_nth_bit(key_down, LEFT_ARROW)) {
        dv.x -= 1;
    }
    if (get_nth_bit(key_down, RIGHT_ARROW)) {
        dv.x += 1;
    }
    if (get_nth_bit(key_down, DOWN_ARROW)) {
        dv.y -= 1;
    }
    if (get_nth_bit(key_down, UP_ARROW)) {
        dv.y += 1;
    }

    vector_t bvel = body_get_velocity(body);
    if (dv.x != 0.0 || dv.y != 0.0) {
        dv = vec_normalize(dv);
        body_set_acceleration(body, vec_multiply(PLAYER_ACCELERATION, dv));
    } else {
        body_set_acceleration(body, vec_multiply(PLAYER_SPACE_FRICTION, vec_negate(bvel)));
    }
    if (vec_norm(bvel) > PLAYER_VELOCITY) {
        body_set_velocity(body, vec_multiply(PLAYER_VELOCITY, vec_normalize(bvel)));
    }

    // enforce bounds
    // TODO: stop the player from going off-screen
    const list_t *shape = body_borrow_shape(body);
    vector_t vel = body_get_velocity(body);
    if ((find_collision(shape, body_borrow_shape(left_bound)).collided && vel.x < 0) ||
        (find_collision(shape, body_borrow_shape(right_bound)).collided && vel.x > 0) ||
        (find_collision(shape, body_borrow_shape(bottom_bound)).collided && vel.y < 0) ||
        (find_collision(shape, body_borrow_shape(top_bound)).collided && vel.y > 0)) {
        body_set_velocity(body, VEC_ZERO);
        body_set_acceleration(body, VEC_ZERO);
    }
}

// TODO: player can still shoot continuously
void shoot_handle(scene_t *scene, body_t *player, body_t *bound, double *bullet_time, size_t key_down) {
    if (get_nth_bit(key_down, SPACE_BAR)) {
        if (*bullet_time > BULLET_COOLDOWN) {
            spawn_bullet(scene, player, bound);
            *bullet_time = 0;
        }
    }
}

void game_loop() {
    scene_t *scene = scene_init();

    sdl_on_key((key_handler_t)on_key_game);

    // using ASTEROID_RADIUS for bounds because it's maximum size
    body_t *top_bound = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MAX.y + 2 * ASTEROID_RADIUS_MAX), SDL_MAX.x, ASTEROID_RADIUS_MAX), INFINITY, COLOR_BLACK);
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

    // health bar
    body_t *health_bar_background = body_health_bar_background_init();
    body_t *health_bar = body_health_bar_init();
    scene_add_body(scene, health_bar_background);
    scene_add_body(scene, health_bar);

    // player
    body_t *player = body_init_player(health_bar);
    body_set_manual_acceleration(player, true);
    scene_add_body(scene, player);
    aster_aux_t *player_aux = body_get_info(player);

    // keypress aux
    game_keypress_aux_t *game_keypress_aux = malloc(sizeof(game_keypress_aux_t));
    game_keypress_aux->scene = scene;
    game_keypress_aux->player = player;
    game_keypress_aux->key_down = 0;
    game_keypress_aux->window = GAME;

    size_t frame = 0;
    double ast_time = 0;
    double bullet_time = BULLET_COOLDOWN; // time since last bullet being fired

    bool to_menu = false;

    while (!sdl_is_done(game_keypress_aux)) {
        double dt = time_since_last_tick();
        ast_time += dt;
        bullet_time += dt;

        //ast_time completely resets when an asteroid spawns
        //and decreases by half when it's met without an asteroid spawning
        if (ast_time >= ASTEROID_SPAWN_RATE) {
            ast_time = ASTEROID_SPAWN_RATE / 2;
            double spawn_chance = drand48();

            if (spawn_chance < ASTEROID_SPAWN_CHANCE) {
                ast_time = 0;
                spawn_asteroid(scene, left_bound, right_bound, top_bound, bottom_bound);
            }
        }

        if (player_aux->game_over) {
            to_menu = true;
            break;
        }

        /* if (frame % DEBUG_PRINT_RATE == 0) {
            print_bits(game_keypress_aux->key_down);
        } */

        velocity_handle(player, game_keypress_aux->key_down, left_bound, right_bound, top_bound, bottom_bound);
        shoot_handle(scene, player, top_bound, &bullet_time, game_keypress_aux->key_down);

        scene_tick(scene, dt);
        sdl_render_scene_black(scene);
        frame++;
    }

    free(game_keypress_aux);
    scene_free(scene);

    if (to_menu) {
        menu_loop();
    }
}
