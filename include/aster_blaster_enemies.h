#ifndef __ASTER_BLASTER_ENEMIES__
#define __ASTER_BLASTER_ENEMIES__

#include "aster_blaster_imports.h"

body_t *body_init_enemy_saw(vector_t pos, scene_t *scene, body_t *player);

body_t *body_init_enemy_shooter(vector_t pos, scene_t *scene, body_t *player);

void spawn_enemy_saw(scene_t *scene, body_t *player);

void spawn_enemy_shooter(scene_t *scene, body_t *player);

void shooter_enemy_all_shoot(scene_t *scene, body_t *player);

body_t *body_init_enemy_shooter_bullet(scene_t *scene, body_t *player, body_t *shooter);

void spawn_enemy_shooter_bullet(scene_t *scene, body_t *player, body_t *shooter);

#endif // #ifndef __ASTER_BLASTER_SETTINGS__