#ifndef __ASTER_BLASTER_COLLISIONS__
#define __ASTER_BLASTER_COLLISIONS__

#include "aster_blaster_imports.h"

void create_health_collision(body_t *attacker, body_t *player, vector_t axis, void *aux);

void create_destructive_collision_force_single(body_t *body1, body_t *body_immortal, vector_t axis, void *aux);

void create_destructive_collision_single(scene_t *scene, body_t *body1, body_t *body_immortal);

void destroy_at_bounds(scene_t *scene, body_t *body, game_bounds_t bounds);

void create_special_collision_force(body_t *ast, body_t *player, vector_t axis, void *aux);

void create_aster_player_collision(scene_t *scene, body_t *ast, body_t *player);

void create_aster_bullet_collision(scene_t *scene, body_t *ast, body_t *bullet);

void create_star_collision_force(body_t *star, body_t *bound, vector_t axis, void *aux);

void create_star_collision(scene_t *scene, body_t *star, body_t *bound);

#endif // #ifndef __ASTER_BLASTER_COLLISIONS__