#ifndef __ASTER_BLASTER_UTILS__
#define __ASTER_BLASTER_UTILS__

#include "aster_blaster_imports.h"

vector_t get_pos_radius_off_screen(double radius);

void print_bits(unsigned int num);

void velocity_handle(body_t *body, size_t key_down, game_bounds_t bounds);

void shoot_handle(scene_t *scene, body_t *player, double *bullet_time, size_t key_down, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list);

double rate_variant(double rate);

#endif // #ifndef __ASTER_BLASTER_UTILS__