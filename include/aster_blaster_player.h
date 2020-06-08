#ifndef __ASTER_BLASTER_PLAYER__
#define __ASTER_BLASTER_PLAYER__

#include "aster_blaster_imports.h"

body_t *body_init_player(body_t *health_bar);

body_t *body_init_bullet(body_t *player);

void spawn_bullet(scene_t *scene, body_t *player, game_bounds_t bounds);

body_t *body_health_bar_background_init();

body_t *body_health_bar_init();

#endif // #ifndef __ASTER_BLASTER_PLAYER_