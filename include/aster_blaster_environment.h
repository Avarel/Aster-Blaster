#ifndef __ASTER_BLASTER_ENVIRONMENT__
#define __ASTER_BLASTER_ENVIRONMENT__

#include "aster_blaster_imports.h"

void spawn_asteroid(scene_t *scene, game_bounds_t bounds, SDL_Texture *tex);

void create_background_stars(scene_t *scene, body_t *bound);

body_t *body_init_black_hole(vector_t pos, scene_t *scene, game_bounds_t bounds);

void spawn_black_hole(scene_t *scene, game_bounds_t bounds);

#endif // #ifndef __ASTER_BLASTER_ENVIRONMENT__