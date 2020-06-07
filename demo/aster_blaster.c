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
#include <SDL2/SDL_image.h>

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
void create_health_collision(body_t *attacker, body_t *player, vector_t axis, void *aux) {
    aster_aux_t *aster_aux = body_get_info(player);

    aster_aux->health = fmax(0, aster_aux->health - body_get_mass(attacker) * DAMAGE_PER_MASS);
    // relies on two right points being idx 1 and 2
    list_t *health_bar_shape = body_get_shape(aster_aux->health_bar);
    vector_t *bottom_right_point = list_get(health_bar_shape, 1);
    vector_t *top_right_point = list_get(health_bar_shape, 2);
    *bottom_right_point = vec(HEALTH_BAR_POS.x + HEALTH_BAR_W * (aster_aux->health / HEALTH_TOTAL), HEALTH_BAR_POS.y);
    *top_right_point = vec(HEALTH_BAR_POS.x + HEALTH_BAR_W * (aster_aux->health / HEALTH_TOTAL), HEALTH_BAR_POS.y + HEALTH_BAR_H);

    if (is_close(aster_aux->health, 0)) {
        aster_aux->game_over = true;
    }
}

void create_destructive_collision_force_single(body_t *body1, body_t *body_immortal, vector_t axis, void *aux) {
    body_remove(body1);
}

void create_destructive_collision_single(scene_t *scene, body_t *body1, body_t *body_immortal) {
    create_collision(scene, body1, body_immortal, create_destructive_collision_force_single, NULL, NULL);
}

void destroy_at_bounds(scene_t *scene, body_t *body, game_bounds_t bounds) {
    create_destructive_collision_single(scene, body, bounds.left);
    create_destructive_collision_single(scene, body, bounds.right);
    create_destructive_collision_single(scene, body, bounds.top);
    create_destructive_collision_single(scene, body, bounds.bottom);
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
    game_bounds_t bounds,
    SDL_Texture *tex) {
    // TODO: random later
    size_t num_sides = 10; //irand_range(ASTEROID_SIDES_MIN, ASTEROID_SIDES_MAX);
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
    double ast_omega = drand_range(-2.0 * M_PI, 2.0 * M_PI);

    aster_aux_t *asteroid_aux = malloc(sizeof(aster_aux_t));
    asteroid_aux->body_type = ASTEROID;

    render_info_t texture = render_texture(tex, ast_radius * 2, ast_radius * 2);

    list_t *aster_shape = polygon_reg_ngon(ast_center, ast_radius, num_sides);
    body_t *asteroid = body_init_texture_with_info(aster_shape, ASTEROID_MASS, texture, asteroid_aux, free);
    body_set_velocity(asteroid, ast_velocity);
    body_set_omega(asteroid, ast_omega);

    scene_add_body(scene, asteroid);
    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_aster_bullet_collision(scene, asteroid, other_body);
            } else if (other_aux->body_type == PLAYER) {
                create_aster_player_collision(scene, asteroid, other_body);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, asteroid, other_body, true);
                create_destructive_collision_single(scene, asteroid, other_body);
            }
        }
    }
    destroy_at_bounds(scene, asteroid, bounds);
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

void spawn_bullet(
    scene_t *scene,
    body_t *player,
    game_bounds_t bounds) {

    body_t *bullet = body_init_bullet(player);

    destroy_at_bounds(scene, bullet, bounds);

    scene_add_body(scene, bullet);

    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == ASTEROID || other_aux->body_type == ENEMY_SAW || other_aux->body_type == ENEMY_SHOOTER) {
                create_aster_bullet_collision(scene, other_body, bullet);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, bullet, other_body, true);
                create_destructive_collision_single(scene, bullet, other_body);
            }
        }
    }
}

/********************
 * BLACK HOLES
 ********************/
body_t *body_init_black_hole(
    vector_t pos,
    scene_t *scene,
    game_bounds_t bounds) {

    list_t *shape = polygon_reg_ngon(pos, BLACK_HOLE_RADIUS, BLACK_HOLE_POINTS);
    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = BLACK_HOLE;
    body_t *black_hole = body_init_with_info(shape, BLACK_HOLE_MASS, BLACK_HOLE_COLOR, aster_aux, free);

    //if the black hole spawns at the left of the screen, x velocity should be
    //positive, so theta between 3*pi/2 and 2*pi
    //otherwise, theta between pi and 3*pi/2, so x velocity is negative
    double theta = 0;
    double midpoint = (SDL_MAX.x - SDL_MIN.x) / 2;
    if (body_get_centroid(black_hole).x < midpoint) {
        theta = drand_range(3 * M_PI / 2, 2 * M_PI);
    } else {
        theta = drand_range(M_PI, 3 * M_PI / 2);
    }
    vector_t bh_velocity = vec(BLACK_HOLE_SPEED * cos(theta), BLACK_HOLE_SPEED * sin(theta));
    body_set_velocity(black_hole, bh_velocity);

    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_newtonian_gravity(scene, G, other_body, black_hole, true);
                create_destructive_collision_single(scene, other_body, black_hole);
            } else if (other_aux->body_type != BLACK_HOLE && other_aux->body_type != PLAYER) {
                create_newtonian_gravity(scene, G, other_body, black_hole, true);
                create_destructive_collision_single(scene, other_body, black_hole);
            } else if (other_aux->body_type == PLAYER) {
                create_newtonian_gravity(scene, G, other_body, black_hole, true);
                create_collision(scene, black_hole, other_body, create_health_collision, NULL, NULL);
            }
        }
    }

    destroy_at_bounds(scene, black_hole, bounds);

    return black_hole;
}

void spawn_black_hole(scene_t *scene, game_bounds_t bounds) {
    double bh_x = drand_range(SDL_MIN.x, SDL_MAX.x);
    vector_t bh_center = vec(bh_x, SDL_MAX.y + BLACK_HOLE_RADIUS);
    body_t *black_hole = body_init_black_hole(bh_center, scene, bounds);
    scene_add_body(scene, black_hole);
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

body_t *body_init_enemy_saw(vector_t pos, scene_t *scene, body_t *player) {
    list_t *shape = polygon_star(pos, ENEMY_SAW_OUT_RADIUS, ENEMY_SAW_IN_RADIUS, ENEMY_SAW_POINTS);

    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = ENEMY_SAW;

    body_t *saw_enemy = body_init_with_info(shape, ENEMY_SAW_MASS, ENEMY_SAW_COLOR, aster_aux, free);

    body_set_omega(saw_enemy, ENEMY_SAW_OMEGA);

    create_attraction(scene, ENEMY_SAW_A, saw_enemy, player, true);
    create_physics_collision(scene, ENEMY_SAW_ELASTICITY, saw_enemy, player);
    create_collision(scene, saw_enemy, player, create_health_collision, NULL, NULL);

    // TODO: similar code in spawn_asteroid, can factor out
    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_aster_bullet_collision(scene, saw_enemy, other_body);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, saw_enemy, other_body, true);
                create_destructive_collision_single(scene, saw_enemy, other_body);
            }
        }
    }

    return saw_enemy;
}

void spawn_enemy_saw(scene_t *scene, body_t *player) {
    body_t *saw_enemy = body_init_enemy_saw(vec(SDL_MAX.x / 2, 0.8 * SDL_MAX.y), scene, player);
    scene_add_body(scene, saw_enemy);
}

body_t *body_init_enemy_shooter(vector_t pos, scene_t *scene, body_t *player) {
    list_t *shape = polygon_ngon_sector(pos, ENEMY_SHOOTER_RADIUS, ENEMY_SHOOTER_POINTS, ENEMY_SHOOTER_SECTOR_POINTS, ENEMY_SHOOTER_INIT_ANGLE);

    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = ENEMY_SHOOTER;

    body_t *shooter_enemy = body_init_with_info(shape, ENEMY_SHOOTER_MASS, ENEMY_SHOOTER_COLOR, aster_aux, free);

    create_attraction_mirrored(scene, ENEMY_SHOOTER_A, shooter_enemy, player, SDL_MAX);
    create_pointing_force(scene, shooter_enemy, player);

    // TODO: similar code in spawn_asteroid, can factor out
    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_aster_bullet_collision(scene, shooter_enemy, other_body);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, shooter_enemy, other_body, true);
                create_destructive_collision_single(scene, shooter_enemy, other_body);
            }
        }
    }

    return shooter_enemy;
}

void spawn_enemy_shooter(scene_t *scene, body_t *player) {
    body_t *shooter_enemy = body_init_enemy_shooter(vec(SDL_MAX.x / 4, 0.8 * SDL_MAX.y), scene, player);
    scene_add_body(scene, shooter_enemy);
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
    sdl_set_background_color(COLOR_WHITE);
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
    game_bounds_t bounds) {
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
    if ((find_collision(shape, body_borrow_shape(bounds.left)).collided && vel.x < 0) ||
        (find_collision(shape, body_borrow_shape(bounds.right)).collided && vel.x > 0) ||
        (find_collision(shape, body_borrow_shape(bounds.bottom)).collided && vel.y < 0) ||
        (find_collision(shape, body_borrow_shape(bounds.top)).collided && vel.y > 0)) {
        body_set_velocity(body, VEC_ZERO);
        body_set_acceleration(body, VEC_ZERO);
    }
}

// TODO: player can still shoot continuously
void shoot_handle(scene_t *scene,
                  body_t *player,
                  double *bullet_time,
                  size_t key_down,
                  game_bounds_t bounds) {
    if (get_nth_bit(key_down, SPACE_BAR)) {
        if (*bullet_time > BULLET_COOLDOWN) {
            spawn_bullet(scene, player, bounds);
            *bullet_time = 0;
        }
    }
}

void game_loop() {
    sdl_set_background_color(COLOR_BLACK);

    scene_t *scene = scene_init();

    sdl_on_key((key_handler_t)on_key_game);

    // using ASTEROID_RADIUS for bounds because it's maximum size
    game_bounds_t bounds = (game_bounds_t){
        .top = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MAX.y + 2 * ASTEROID_RADIUS_MAX), SDL_MAX.x, ASTEROID_RADIUS_MAX), INFINITY, COLOR_BLACK),
        .bottom = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MIN.y - 3 * ASTEROID_RADIUS_MAX), SDL_MAX.x, ASTEROID_RADIUS_MAX), INFINITY, COLOR_BLACK),
        .left = body_init(polygon_rect(vec(SDL_MIN.x - 3 * ASTEROID_RADIUS_MAX, SDL_MIN.y), ASTEROID_RADIUS_MAX, SDL_MAX.y), INFINITY, COLOR_BLACK),
        .right = body_init(polygon_rect(vec(SDL_MAX.x + ASTEROID_RADIUS_MAX, SDL_MIN.y), ASTEROID_RADIUS_MAX, SDL_MAX.y), INFINITY, COLOR_BLACK)};

    scene_add_body(scene, bounds.left);
    scene_add_body(scene, bounds.right);
    scene_add_body(scene, bounds.top);
    scene_add_body(scene, bounds.bottom);

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
    double bh_time = 0;
    double bullet_time = BULLET_COOLDOWN; // time since last bullet being fired

    // TODO: enemy spawning mechanics
    spawn_enemy_saw(scene, player);
    spawn_enemy_shooter(scene, player);

    spawn_black_hole(scene, bounds);

    bool to_menu = false;

    SDL_Texture *tex = sdl_load_texture("./asteroid.png");

    while (!sdl_is_done(game_keypress_aux)) {
        double dt = time_since_last_tick();
        ast_time += dt;
        bh_time += dt;
        bullet_time += dt;

        //ast_time completely resets when an asteroid spawns
        //and decreases by half when it's met without an asteroid spawning
        if (ast_time >= ASTEROID_SPAWN_RATE) {
            ast_time = ASTEROID_SPAWN_RATE / 2;
            double spawn_chance = drand48();

            if (spawn_chance < ASTEROID_SPAWN_CHANCE) {
                ast_time = 0;
                spawn_asteroid(scene, bounds, tex);
            }
        }

        // bh_time completely resets when an black hole spawns
        // and decreases by half when it's met without an black hole spawning
        if (bh_time >= BLACK_HOLE_SPAWN_RATE) {
            bh_time = BLACK_HOLE_SPAWN_RATE / 2;
            double spawn_chance = drand48();

            if (spawn_chance < BLACK_HOLE_SPAWN_CHANCE) {
                bh_time = 0;
                spawn_black_hole(scene, bounds);
            }
        }

        if (player_aux->game_over) {
            to_menu = true;
            break;
        }

        /* if (frame % DEBUG_PRINT_RATE == 0) {
            print_bits(game_keypress_aux->key_down);
        } */

        velocity_handle(player, game_keypress_aux->key_down, bounds);
        shoot_handle(scene, player, &bullet_time, game_keypress_aux->key_down, bounds);

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        frame++;
    }

    free(game_keypress_aux);
    scene_free(scene);
    SDL_DestroyTexture(tex);

    if (to_menu) {
        menu_loop();
    }
}
