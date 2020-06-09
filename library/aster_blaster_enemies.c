#include "aster_blaster_imports.h"

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
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_destructive_collision(scene, saw_enemy, other_body);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, saw_enemy, other_body, true);
                create_destructive_collision_single(scene, saw_enemy, other_body);
            }
        }
    }

    return saw_enemy;
}

body_t *body_init_enemy_shooter(vector_t pos, scene_t *scene, body_t *player) {
    list_t *shape = polygon_ngon_sector(pos, ENEMY_SHOOTER_RADIUS, ENEMY_SHOOTER_POINTS, ENEMY_SHOOTER_SECTOR_POINTS, ENEMY_SHOOTER_INIT_ANGLE);

    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = ENEMY_SHOOTER;

    body_t *shooter_enemy = body_init_with_info(shape, ENEMY_SHOOTER_MASS, ENEMY_SHOOTER_COLOR, aster_aux, free);

    create_attraction_mirrored(scene, ENEMY_SHOOTER_A, shooter_enemy, player, SDL_MAX, rand_vec(vec(-3 * ENEMY_SHOOTER_RADIUS, -3 * ENEMY_SHOOTER_RADIUS), vec(3 * ENEMY_SHOOTER_RADIUS, 3 * ENEMY_SHOOTER_RADIUS)));
    create_pointing_force(scene, shooter_enemy, player);

    // TODO: similar code in spawn_asteroid, can factor out
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_destructive_collision(scene, shooter_enemy, other_body);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, shooter_enemy, other_body, true);
                create_destructive_collision_single(scene, shooter_enemy, other_body);
            }
        }
    }

    return shooter_enemy;
}

// TODO: offsets so they don't stack
void spawn_enemy_saw(scene_t *scene, body_t *player) {
    body_t *saw_enemy = body_init_enemy_saw(get_pos_radius_off_screen(ENEMY_SAW_OUT_RADIUS), scene, player);
    scene_add_body(scene, saw_enemy);
}

void spawn_enemy_shooter(scene_t *scene, body_t *player) {
    body_t *shooter_enemy = body_init_enemy_shooter(get_pos_radius_off_screen(ENEMY_SHOOTER_RADIUS), scene, player);
    scene_add_body(scene, shooter_enemy);
}

void shooter_enemy_all_shoot(scene_t *scene, body_t *player, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list) {
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);
        aster_aux_t *aux = body_get_info(body);
        if (aux && aux->body_type == ENEMY_SHOOTER) {
            spawn_enemy_shooter_bullet(scene, player, body, bounds, ast_sprites_list);
        }
    }
}

body_t *body_init_enemy_shooter_bullet(scene_t *scene, body_t *player, body_t *shooter, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list) {
    vector_t bullet_center = body_get_centroid(shooter); // calculate pos
    vector_t direction = vec_x(1);
    direction = vec_rotate(direction, angle_to(bullet_center, body_get_centroid(player)));
    bullet_center = vec_add(bullet_center, vec_multiply(ENEMY_SHOOTER_RADIUS, direction));

    list_t *shape = polygon_star(bullet_center, ENEMY_SHOOTER_BULLET_OUT_RADIUS, ENEMY_SHOOTER_BULLET_IN_RADIUS, ENEMY_SHOOTER_BULLET_POINTS);
    aster_aux_t *aux = malloc(sizeof(aster_aux_t));
    aux->body_type = ENEMY_SHOOTER_BULLET;
    body_t *bullet = body_init_with_info(shape, ENEMY_SHOOTER_BULLET_MASS, ENEMY_SHOOTER_BULLET_COLOR, aux, free);

    body_set_velocity(bullet, vec_multiply(ENEMY_SHOOTER_BULLET_SPEED, direction));

    create_collision(scene, bullet, player, create_health_collision, NULL, NULL);
    create_destructive_collision_single(scene, bullet, player);

    // TODO: partial code duplication from palyer bullet
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == ASTEROID) {
                create_aster_bullet_collision(scene, other_body, bullet, bounds, ast_sprites_list);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, bullet, other_body, true);
                create_destructive_collision_single(scene, bullet, other_body);
            }
        }
    }

    destroy_at_bounds(scene, bullet, bounds);

    return bullet;
}

void spawn_enemy_shooter_bullet(scene_t *scene, body_t *player, body_t *shooter, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list) {
    body_t *bullet = body_init_enemy_shooter_bullet(scene, player, shooter, bounds, ast_sprites_list);
    scene_add_body(scene, bullet);
}

body_t *body_init_boss(scene_t *scene, body_t *health_bar, body_t *movement_trigger, body_t *right_trigger, body_t *left_trigger) {
    list_t *boss_shape =  polygon_star(BOSS_INIT_POS, BOSS_OUT_RADIUS, BOSS_IN_RADIUS, BOSS_POINTS);

    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = BOSS;
    aster_aux->health = BOSS_HEALTH;
    aster_aux->health_bar = health_bar;
    aster_game_over->false;

    body_t *boss = body_init_with_info(boss_shape, BOSS_MASS, BOSS_COLOR, aster_aux, free);

    body_set_omega(boss, BOSS_OMEGA);

    // TODO: similar code in spawn_asteroid, can factor out
    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_destructive_collision(scene, saw_enemy, other_body);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, saw_enemy, other_body, true);
                create_destructive_collision_single(scene, saw_enemy, other_body);
            }
        }
    }

    return saw_enemy;
}

void spawn_boss(scene_t *scene, body_t *movement_trigger, body_t *right_trigger, body_t *left_trigger);

body_t *body_init_boss_bomb(scene_t *scene, bosy_t *boss, game_bounds_t bound);

void spawn_boss_bomb(scene_t *scene, bosy_t *boss, game_bounds_t bound);

body_t *body_init_boss_bullet(scene_t *scene, bosy_t *bomb, game_bounds_t bound);

void spawn_boss_bullet(scene_t *scene, bosy_t *bomb, game_bounds_t bound);
