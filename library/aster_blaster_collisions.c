#include "aster_blaster_imports.h"

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

typedef struct aster_bullet_collision_aux {
    scene_t *scene;
    game_bounds_t bounds;
    ast_sprites_list_t ast_sprites_list;
} aster_bullet_collision_aux_t;

void create_aster_fragments(body_t *ast, body_t *bullet, vector_t axis, void *aux) {
    if (body_is_removed(ast) || body_is_removed(bullet)) return;
    aster_bullet_collision_aux_t *caux = (aster_bullet_collision_aux_t *) aux;
    double mass = body_get_mass(ast) / 1.5;
    vector_t velocity = body_get_velocity(ast);
    vector_t centroid = body_get_centroid(ast);
    body_remove(bullet);
    body_remove(ast);
    if (mass > ASTEROID_MIN_MASS) {
        // todo make it better
        spawn_asteroid_general(caux->scene, mass, centroid, vec_rotate(velocity, 1.0), caux->bounds, caux->ast_sprites_list);
        spawn_asteroid_general(caux->scene, mass, centroid, vec_rotate(velocity, -1.0), caux->bounds, caux->ast_sprites_list);
    }
}

void create_aster_bullet_collision(scene_t *scene, body_t *ast, body_t *bullet, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list) {
    aster_bullet_collision_aux_t *aux = malloc(sizeof(aster_bullet_collision_aux_t));
    aux->scene = scene;
    aux->bounds = bounds;
    aux->ast_sprites_list = ast_sprites_list;
    create_collision(scene, ast, bullet, create_aster_fragments, aux, free);
}

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