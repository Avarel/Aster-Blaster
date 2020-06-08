#include "aster_blaster_imports.h"

void print_bits(unsigned int num) {
    for (int bit = 0; bit < (sizeof(unsigned int) * 2); bit++) {
        printf("%i ", num & 0x01);
        num = num >> 1;
    }
    printf("\n");
}

void velocity_handle(body_t *body, size_t key_down, game_bounds_t bounds) {
    // handle movement
    vector_t dv = VEC_ZERO;
    if (get_nth_bit(key_down, LEFT_ARROW)) {
        dv.x -= 1;
    }
    if (get_nth_bit(key_down, RIGHT_ARROW)) {
        dv.x += 1;
    }
    if (get_nth_bit(key_down, DOWN_ARROW)) {
        dv.y -= 1;
    }
    if (get_nth_bit(key_down, UP_ARROW)) {
        dv.y += 1;
    }

    vector_t bvel = body_get_velocity(body);
    if (dv.x != 0.0 || dv.y != 0.0) {
        dv = vec_normalize(dv);
        body_set_acceleration(body, vec_multiply(PLAYER_ACCELERATION, dv));
    } else {
        body_set_acceleration(body, vec_multiply(PLAYER_SPACE_FRICTION, vec_negate(bvel)));
    }
    if (vec_norm(bvel) > PLAYER_VELOCITY) {
        body_set_velocity(body, vec_multiply(PLAYER_VELOCITY, vec_normalize(bvel)));
    }

    // enforce bounds
    // TODO: stop the player from going off-screen
    const list_t *shape = body_borrow_shape(body);
    vector_t vel = body_get_velocity(body);
    if ((find_collision(shape, body_borrow_shape(bounds.left)).collided && vel.x < 0) ||
        (find_collision(shape, body_borrow_shape(bounds.right)).collided && vel.x > 0) ||
        (find_collision(shape, body_borrow_shape(bounds.bottom)).collided && vel.y < 0) ||
        (find_collision(shape, body_borrow_shape(bounds.top)).collided && vel.y > 0)) {
        body_set_velocity(body, VEC_ZERO);
        body_set_acceleration(body, VEC_ZERO);
    }
}

void shoot_handle(scene_t *scene,
                  body_t *player,
                  double *bullet_time,
                  size_t key_down,
                  game_bounds_t bounds) {
    if (get_nth_bit(key_down, SPACE_BAR)) {
        if (*bullet_time > BULLET_COOLDOWN) {
            spawn_bullet(scene, player, bounds);
            *bullet_time = 0;
        }
    }
}

double rate_variant(double rate) {
    return rate * drand_range(RATE_VARIANT_LOWER, RATE_VARIANT_UPPER);
}