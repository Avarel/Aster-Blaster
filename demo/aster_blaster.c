#include "aster_blaster_imports.h"

void menu_loop();
void game_loop();
void control_loop(); // TODO: later

int main() {
    sdl_init(SDL_MIN, SDL_MAX);
    sdl_set_font(&FONT_PATH_ASTER_BLASTER[0]);
    init_random();
    menu_loop();
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
    text_box_t *menu_game_start_text_box = text_box_init(&MENU_GAME_START_TEXT[0], MENU_GAME_START_FONT_SIZE, MENU_GAME_START_ORIGIN, MENU_GAME_START_JUSTIFICATION);

    scene_add_text_box(scene, menu_title_text_box);
    scene_add_text_box(scene, menu_game_start_text_box);

    bool to_game = false;
    while (!sdl_is_done(menu_keypress_aux)) {
        double dt = time_since_last_tick();

        if (menu_keypress_aux->window == GAME) {
            to_game = true;
            break;
        }

        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    free(menu_keypress_aux);
    scene_free(scene);

    if (to_game) {
        game_loop();
    }
}

void victory_loop() {
    sdl_set_background_color(COLOR_WHITE);
    scene_t *scene = scene_init();

    sdl_on_key((key_handler_t)on_key_victory);

    menu_keypress_aux_t *menu_keypress_aux = malloc(sizeof(menu_keypress_aux_t));
    menu_keypress_aux->scene = scene;
    menu_keypress_aux->key_down = 0;
    menu_keypress_aux->window = VICTORY;

    text_box_t *victory_text_box = text_box_init(&VICTORY_TEXT[0], VICTORY_FONT_SIZE, VICTORY_ORIGIN, VICTORY_JUSTIFICATION);
    text_box_t *menu_game_start_text_box = text_box_init(&MENU_GAME_START_TEXT[0], MENU_GAME_START_FONT_SIZE, MENU_GAME_START_ORIGIN, MENU_GAME_START_JUSTIFICATION);

    scene_add_text_box(scene, victory_text_box);
    scene_add_text_box(scene, menu_game_start_text_box);

    bool to_game = false;
    while (!sdl_is_done(menu_keypress_aux)) {
        double dt = time_since_last_tick();

        if (menu_keypress_aux->window == GAME) {
            to_game = true;
            break;
        }

        scene_tick(scene, dt);
        sdl_render_scene(scene);
    }

    free(menu_keypress_aux);
    scene_free(scene);

    if (to_game) {
        game_loop();
    }
}

void game_loop() {
    sdl_set_background_color(COLOR_BLACK);

    scene_t *scene = scene_init();

    sdl_on_key((key_handler_t)on_key_game);

    // using ASTEROID_RADIUS for bounds because it's maximum size
    // TODO: make a new variable for this?
    game_bounds_t bounds = (game_bounds_t){
        .top = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MAX.y + 2 * ASTEROID_RADIUS_MAX), SDL_MAX.x, ASTEROID_RADIUS_MAX), INFINITY, COLOR_BLACK),
        .bottom = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MIN.y - 3 * ASTEROID_RADIUS_MAX), SDL_MAX.x, ASTEROID_RADIUS_MAX), INFINITY, COLOR_BLACK),
        .left = body_init(polygon_rect(vec(SDL_MIN.x - 3 * ASTEROID_RADIUS_MAX, SDL_MIN.y), ASTEROID_RADIUS_MAX, SDL_MAX.y), INFINITY, COLOR_BLACK),
        .right = body_init(polygon_rect(vec(SDL_MAX.x + ASTEROID_RADIUS_MAX, SDL_MIN.y), ASTEROID_RADIUS_MAX, SDL_MAX.y), INFINITY, COLOR_BLACK)};

    scene_add_body(scene, bounds.left);
    scene_add_body(scene, bounds.right);
    scene_add_body(scene, bounds.top);
    scene_add_body(scene, bounds.bottom);

    // Boss movement triggers (made before stars so they aren't seen)
    list_t *boss_movement_trigger_shape = polygon_rect(vec(SDL_MIN.x, 0.5 * SDL_MAX.y), SDL_MAX.x, BOSS_OUT_RADIUS);
    body_t *boss_movement_trigger = body_init(boss_movement_trigger_shape, INFINITY, COLOR_BLACK);
    list_t *boss_right_trigger_shape = polygon_rect(vec(SDL_MIN.x, SDL_MIN.y), 0.125 * SDL_MAX.x, SDL_MAX.y);
    body_t *boss_right_trigger = body_init(boss_right_trigger_shape, INFINITY, COLOR_BLACK);
    list_t *boss_left_trigger_shape = polygon_rect(vec(0.875 * SDL_MAX.x, SDL_MIN.y), 0.125 * SDL_MAX.x, SDL_MAX.y);
    body_t *boss_left_trigger = body_init(boss_left_trigger_shape, INFINITY, COLOR_BLACK);
    scene_add_body(scene, boss_movement_trigger);
    scene_add_body(scene, boss_left_trigger);
    scene_add_body(scene, boss_right_trigger);

    // when stars collide with this offscreen they'll teleport to just off the
    // top of the scene so it loops
    // TODO: magic numbers
    body_t *star_bound = body_init(polygon_rect(vec(SDL_MIN.x, SDL_MIN.y - 6 * STAR_RADIUS_MAX), SDL_MAX.x, 2 * STAR_RADIUS_MAX), INFINITY, COLOR_BLACK);
    create_background_stars(scene, star_bound);

    // health bar
    body_t *health_bar_background = body_health_bar_background_init();
    body_t *health_bar = body_health_bar_init();
    scene_add_body(scene, health_bar_background);
    scene_add_body(scene, health_bar);

    SDL_Texture *player_texture = sdl_load_texture("./assets/ship.png");
    // player
    body_t *player = body_init_player(health_bar, render_texture(player_texture, PLAYER_RADIUS * 1.5, PLAYER_RADIUS * 1.2));
    body_set_manual_acceleration(player, true);
    scene_add_body(scene, player);
    aster_aux_t *player_aux = body_get_info(player);

    // keypress aux
    game_keypress_aux_t *game_keypress_aux = malloc(sizeof(game_keypress_aux_t));
    game_keypress_aux->scene = scene;
    game_keypress_aux->player = player;
    game_keypress_aux->key_down = 0;
    game_keypress_aux->window = GAME;


    bool boss_tangible = false;

    size_t frame = 0;
    double ast_time = 0;
    double bh_time = 0;
    double bullet_time = BULLET_COOLDOWN; // time since last bullet being fired
    double saw_time = 0;
    double shooter_spawn_time = 0;
    double shooter_shot_time = 0;
    double boss_spawn_time = 0;
    double boss_shot_time = 0;

    bool to_menu = false;
    bool to_victory = false;

    ast_sprites_list_t ast_sprites_list = {
        .circle = sdl_load_texture("./assets/circle.png"),
        .heptagon = sdl_load_texture("./assets/heptagon.png"),
        .hexagon = sdl_load_texture("./assets/hexagon.png"),
        .pentagon = sdl_load_texture("./assets/pentagon.png"),
        // .ship = sdl_load_texture("./assets/ship.png"),
    };

    double saw_spawn_rate = rate_variant(ENEMY_SAW_SPAWN_RATE);
    double shooter_spawn_rate = rate_variant(ENEMY_SHOOTER_SPAWN_RATE);
    double shooter_shot_rate = rate_variant(ENEMY_SHOOTER_SHOT_RATE);
    double boss_shot_rate = rate_variant(BOSS_SHOT_RATE);
    body_t *boss = NULL;
    aster_aux_t *boss_aux = NULL;

    while (!sdl_is_done(game_keypress_aux)) {
        double dt = time_since_last_tick();
        ast_time += dt;
        bh_time += dt;
        bullet_time += dt;
        saw_time += dt;
        shooter_spawn_time += dt;
        shooter_shot_time += dt;
        boss_spawn_time += dt;
        boss_shot_time += dt;

        if (boss_spawn_time >= BOSS_SPAWN_TIME && !boss_tangible) {
            spawn_boss(scene, boss_movement_trigger, boss_left_trigger, boss_right_trigger, &boss_tangible);
            boss_spawn_time = 0;
            boss_shot_time = 0;
            boss = scene_get_body(scene, scene_bodies(scene) - 1);
            boss_aux = body_get_info(boss);
        }

        //ast_time completely resets when an asteroid spawns
        //and decreases by half when it's met without an asteroid spawning
        if (ast_time >= ASTEROID_SPAWN_RATE) {
            ast_time = ASTEROID_SPAWN_RATE / 2;
            double spawn_chance = drand48();

            // If boss is present the chance of spawning is halved and the spawn_chance
            // stops increasing when an asteroid doesn't spawn
            if (boss_tangible) {
                spawn_chance *= 2;
                ast_time = 0;
            }

            if (spawn_chance < ASTEROID_SPAWN_CHANCE) {
                ast_time = 0;
                spawn_asteroid_top(scene, bounds, ast_sprites_list);
            }
        }

        // bh_time completely resets when an black hole spawns
        // and decreases by half when it's met without an black hole spawning
        // Black holes don't spawn when the boss is present.
        if (!boss_tangible && bh_time >= BLACK_HOLE_SPAWN_RATE) {
            bh_time = BLACK_HOLE_SPAWN_RATE / 2;
            double spawn_chance = drand48();
            if (spawn_chance < BLACK_HOLE_SPAWN_CHANCE) {
                bh_time = 0;
                spawn_black_hole(scene, bounds);
            }
        }

        if (saw_time >= saw_spawn_rate) {
            size_t to_spawn = irand_range(ENEMY_SAW_SWARM_SIZE_MIN, ENEMY_SAW_SWARM_SIZE_MAX);
            for (size_t i = 0; i < to_spawn; i++) {
                spawn_enemy_saw(scene, player);
            }
            saw_time = 0;
            saw_spawn_rate = rate_variant(ENEMY_SAW_SPAWN_RATE);
        }

        if (shooter_spawn_time >= shooter_spawn_rate) {
            spawn_enemy_shooter(scene, player);
            shooter_spawn_time = 0;
            shooter_spawn_rate = rate_variant(ENEMY_SHOOTER_SPAWN_RATE);
        }

        if (shooter_shot_time >= shooter_shot_rate) {
            shooter_enemy_all_shoot(scene, player, bounds, ast_sprites_list);
            shooter_shot_time = 0;
            shooter_shot_rate = rate_variant(ENEMY_SHOOTER_SHOT_RATE);
        }

        if (boss_tangible && boss_shot_time >= BOSS_SHOT_RATE) {
            spawn_boss_bomb(scene, bounds, ast_sprites_list);
            boss_shot_time = 0;
            boss_shot_rate = rate_variant(BOSS_SHOT_RATE);
        }

        if (boss_tangible && boss_aux->game_over) {
            to_victory = true;
            break;
        }

        if (player_aux->game_over) {
            to_menu = true;
            break;
        }

        /* if (frame % DEBUG_PRINT_RATE == 0) {
            print_bits(game_keypress_aux->key_down);
        } */

        velocity_handle(player, game_keypress_aux->key_down, bounds);
        shoot_handle(scene, player, &bullet_time, game_keypress_aux->key_down, bounds, ast_sprites_list, boss_tangible);

        scene_tick(scene, dt);
        sdl_render_scene(scene);
        frame++;
    }

    free(game_keypress_aux);
    scene_free(scene);
    destroy_ast_sprite_list(ast_sprites_list);
    SDL_DestroyTexture(player_texture);

    if (to_menu) {
        menu_loop();
    }
    if (to_victory) {
        victory_loop();
    }
}
