#include "aster_blaster_imports.h"

void print_bits(unsigned int num);

void velocity_handle(body_t *body, size_t key_down, game_bounds_t bounds);

void shoot_handle(scene_t *scene, body_t *player, double *bullet_time, size_t key_down, game_bounds_t bounds);

double rate_variant(double rate);