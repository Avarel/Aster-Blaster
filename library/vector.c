#include "vector.h"
#include "utils.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const vector_t VEC_ZERO = (vector_t){.x = 0, .y = 0};

vector_t vec(double x, double y) {
    return (vector_t){.x = x, .y = y};
}

vector_t vec_x(double x) {
    return vec(x, 0);
}

vector_t vec_y(double y) {
    return vec(0, y);
}

vector_t *vec_alloc(vector_t v) {
    vector_t *ptr = malloc(sizeof(vector_t));
    assert(ptr != NULL);
    *ptr = v;
    return ptr;
}

vector_t vec_add(vector_t v1, vector_t v2) {
    return vec(v1.x + v2.x, v1.y + v2.y);
}

vector_t vec_subtract(vector_t v1, vector_t v2) {
    return vec(v1.x - v2.x, v1.y - v2.y);
}

vector_t vec_negate(vector_t v) {
    return vec(-v.x, -v.y);
}

vector_t vec_multiply(double scalar, vector_t v) {
    return vec(scalar * v.x, scalar * v.y);
}

double vec_dot(vector_t v1, vector_t v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

double vec_cross(vector_t v1, vector_t v2) {
    return v1.x * v2.y - v2.x * v1.y;
}

vector_t vec_rotate(vector_t v, double angle) {
    return vec(v.x * cos(angle) - v.y * sin(angle),
               v.x * sin(angle) + v.y * cos(angle));
}

vector_t vec_tick(vector_t vel, vector_t acc, double t) {
    return vec_add(vel, vec_multiply(t, acc));
}

vector_t vec_normalize(vector_t vec) {
    return vec_multiply(1.0 / vec_norm(vec), vec);
}

vector_t vec_perp(vector_t v) {
    return vec(-v.y, v.x);
}

double vec_norm(vector_t vec) {
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}

vector_t *vec_clone(const vector_t *v) {
    return vec_alloc(*v);
}