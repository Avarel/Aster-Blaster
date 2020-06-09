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
    double saw_time = 0;
    double shooter_time = 0;

    spawn_black_hole(scene, bounds);

    bool to_menu = false;

    SDL_Texture *tex = sdl_load_texture("./asteroid.png");

    double saw_spawn_rate = rate_variant(ENEMY_SAW_SPAWN_RATE);
    double shooter_spawn_rate = rate_variant(ENEMY_SHOOTER_SPAWN_RATE);

    while (!sdl_is_done(game_keypress_aux)) {
        double dt = time_since_last_tick();
        ast_time += dt;
        bh_time += dt;
        bullet_time += dt;
        saw_time += dt;
        shooter_time += dt;

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

        if (saw_time >= saw_spawn_rate) {
            size_t to_spawn = irand_range(ENEMY_SAW_SWARM_SIZE_MIN, ENEMY_SAW_SWARM_SIZE_MAX);
            for (size_t i = 0; i < to_spawn; i++) {
                spawn_enemy_saw(scene, player);
            }
            saw_time = 0;
            saw_spawn_rate = rate_variant(ENEMY_SAW_SPAWN_RATE);
        }


        // TODO: give random offset from target so they aren't all on top of each other
        if (shooter_time >= shooter_spawn_rate) {     
            spawn_enemy_shooter(scene, player);
            shooter_time = 0;
            shooter_spawn_rate = rate_variant(ENEMY_SHOOTER_SPAWN_RATE);
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