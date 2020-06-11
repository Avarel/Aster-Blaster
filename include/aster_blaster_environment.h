#ifndef __ASTER_BLASTER_ENVIRONMENT__
#define __ASTER_BLASTER_ENVIRONMENT__

#include "aster_blaster_imports.h"

void spawn_asteroid_top(scene_t *scene, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list);

body_t *spawn_asteroid_general(scene_t *scene, double mass, vector_t ast_center, vector_t ast_velocity, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list);

void create_background_stars(scene_t *scene, body_t *bound);

body_t *body_init_black_hole(vector_t pos, scene_t *scene, game_bounds_t bounds, SDL_Texture *texture);

void spawn_black_hole(scene_t *scene, game_bounds_t bounds, SDL_Texture *texture);

#endif // #ifndef __ASTER_BLASTER_ENVIRONMENT__