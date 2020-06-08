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

void create_aster_bullet_collision(scene_t *scene, body_t *ast, body_t *bullet) {
    // style 2: physics (maybe even reduce mass!)
    // create_destructive_collision_single(scene, bullet, ast);
    // create_physics_collision(scene, 1.0, ast, bullet);

    // style 1: simple
    create_destructive_collision(scene, ast, bullet);
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