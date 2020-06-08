#ifndef __ASTER_BLASTER_ENEMIES__
#define __ASTER_BLASTER_ENEMIES__

#include "aster_blaster_imports.h"

vector_t get_pos_radius_off_screen(double radius);

body_t *body_init_enemy_saw(vector_t pos, scene_t *scene, body_t *player);

body_t *body_init_enemy_shooter(vector_t pos, scene_t *scene, body_t *player);

void spawn_enemy_saw(scene_t *scene, body_t *player);

void spawn_enemy_shooter(scene_t *scene, body_t *player);

#endif // #ifndef __ASTER_BLASTER_SETTINGS__