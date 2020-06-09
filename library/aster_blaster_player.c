#include "aster_blaster_imports.h"

body_t *body_init_player(body_t *health_bar, render_info_t render_info) {
    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = PLAYER;
    aster_aux->health = HEALTH_TOTAL;
    aster_aux->health_bar = health_bar;
    aster_aux->game_over = false;

    list_t *player_shape = polygon_ngon_sector(PLAYER_INIT_POS, PLAYER_RADIUS, PLAYER_SIDES, PLAYER_SECTOR_SIDES, PLAYER_ANGLE);
    body_t *player = body_init_texture_with_info(player_shape, PLAYER_MASS, render_info, aster_aux, free);
    return player;
}

body_t *body_init_bullet(body_t *player) {
    aster_aux_t *aux = malloc(sizeof(aster_aux_t));
    aux->body_type = BULLET;
    list_t *bullet_shape = polygon_reg_ngon(vec_add(body_get_centroid(player), vec_y(PLAYER_RADIUS)), BULLET_RADIUS, 3);
    body_t *bullet = body_init_with_info(bullet_shape, BULLET_MASS, BULLET_COLOR, aux, free);
    body_set_velocity(bullet, BULLET_VELOCITY);
    return bullet;
}

void spawn_bullet(scene_t *scene, body_t *player, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list, bool boss_tangible) {
    body_t *bullet = body_init_bullet(player);

    destroy_at_bounds(scene, bullet, bounds);

    scene_add_body(scene, bullet);

    for (size_t i = 0; i < scene_bodies(scene) - 1; i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == ASTEROID) {
                create_aster_bullet_collision(scene, other_body, bullet, bounds, ast_sprites_list);
            } else if (other_aux->body_type == ENEMY_SAW || other_aux->body_type == ENEMY_SHOOTER) {
                create_destructive_collision(scene, other_body, bullet);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, bullet, other_body, true);
                create_destructive_collision_single(scene, bullet, other_body);
            } else if (other_aux->body_type == BOSS && boss_tangible) {
                create_collision(scene, bullet, other_body, create_boss_health_collision, NULL, NULL);
                create_destructive_collision_single(scene, bullet, other_body);
            }
        }
    }
}

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
