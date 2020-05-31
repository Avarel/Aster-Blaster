#include "body.h"
#include "color.h"
#include "list.h"
#include "math.h"
#include "polygon.h"
#include "utils.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const size_t INITIAL_LIST_SIZE = 2;

typedef struct body {
    double mass;
    list_t *shape;

    vector_t centroid;
    vector_t velocity;
    vector_t acceleration;

    double theta; // Angular displacement
    double omega; // Angular velocity

    bool manual_acceleration;

    rgb_color_t color;

    bool destroy;

    list_t *decals;

    void *aux;
    free_func_t freer;
} body_t;

body_t *body_init(list_t *shape, double mass, rgb_color_t color) {
    body_t *body = malloc(sizeof(body_t));
    assert(body != NULL);

    body->mass = mass;
    body->centroid = polygon_centroid(shape);
    body->shape = shape;

    body->velocity = VEC_ZERO;
    body->acceleration = VEC_ZERO;

    body->theta = 0;
    body->omega = 0;

    body->manual_acceleration = false;

    body->color = color;

    body->decals = list_init(0, NULL); // NULL b/c scene is charged with freeing the actual bodies

    body->destroy = false;
    body->aux = NULL;
    body->freer = NULL;

    return body;
}

/**
 * Allocates memory for a body with the given parameters.
 * The body is initially at rest.
 * Asserts that the mass is positive and that the required memory is allocated.
 *
 * @param shape a list of vectors describing the initial shape of the body
 * @param mass the mass of the body (if INFINITY, stops the body from moving)
 * @param color the color of the body, used to draw it on the screen
 * @param info additional information to associate with the body,
 *   e.g. its type if the scene has multiple types of bodies
 * @param info_freer if non-NULL, a function call on the info to free it
 * @return a pointer to the newly allocated body
 */
body_t *body_init_with_info(
    list_t *shape,
    double mass,
    rgb_color_t color,
    void *info,
    free_func_t info_freer
) {
    body_t *body = body_init(shape, mass, color);

    body->aux = info;
    body->freer = info_freer;

    return body;
}

void body_free(body_t *body) {
    list_free(body->shape);
    list_free(body->decals);
    if (body->aux != NULL && body->freer != NULL) {
        body->freer(body->aux);
    }
    free(body);
}

void body_add_decal(body_t *body, body_t *decal) {
    list_add(body->decals, decal);
}

list_t *body_get_shape(body_t *body) {
    return list_clone(body->shape, (clone_func_t)vec_clone);
}

vector_t body_get_centroid(body_t *body) {
    return body->centroid;
}

vector_t body_get_velocity(body_t *body) {
    return body->velocity;
}

vector_t body_get_acceleration(body_t *body) {
    return body->acceleration;
}

bool body_get_manual_acceleration(body_t *body) {
    return body->manual_acceleration;
}

double body_get_omega(body_t *body) {
    return body->omega;
}

double body_get_mass(body_t *body) {
    return body->mass;
}

rgb_color_t body_get_color(body_t *body) {
    return body->color;
}

void *body_get_info(body_t *body) {
    return body->aux;
}

void body_translate(body_t *body, vector_t translate) {
    body->centroid = vec_add(body->centroid, translate);
    polygon_translate(body->shape, translate);
}

void body_set_centroid(body_t *body, vector_t x) {
    body_translate(body, vec_subtract(x, body_get_centroid(body)));
}

void body_set_velocity(body_t *body, vector_t v) {
    body->velocity = v;
}

void body_set_acceleration(body_t *body, vector_t v) {
    body->acceleration = v;
}

void body_set_manual_acceleration(body_t *body, bool f) {
    body->manual_acceleration = f;
}

void body_set_omega(body_t *body, double o) {
    body->omega = o;
}

void body_rotate(body_t *body, double angle) {
    polygon_rotate(body->shape, angle, body_get_centroid(body));
    body->theta += angle;
}

void body_set_rotation(body_t *body, double angle) {
    body_rotate(body, angle - body->theta);
}

void body_add_force(body_t *body, vector_t force) { // TODO: possible DBZ error
    vector_t delta_acc = vec_multiply(1 / body->mass, force); // a = F/m
    body->acceleration = vec_add(body->acceleration, delta_acc);
}

void body_add_impulse(body_t *body, vector_t impulse) {
    vector_t delta_vel = vec_multiply(1 / body->mass, impulse); // ∆v = ∆p/m
    body->velocity = vec_add(body->velocity, delta_vel);
}

void body_tick(body_t *body, double dt) {
    vector_t vel = vec_tick(body->velocity, body->acceleration, dt);
    body->velocity = vel;
    vector_t translate = vec_multiply(dt, body->velocity);
    body_translate(body, translate);
    double angle = fmod(body->theta + dt * body->omega, 2 * M_PI);
    body_set_rotation(body, angle);
    for (size_t i = 0; i < list_size(body->decals); i++) {
        body_t *decal = list_get(body->decals, i);
        decal->velocity = vel;
        body_translate(decal, translate);
        body_set_rotation(decal, angle);
    } 
}

void body_remove(body_t *body) {
    body->destroy = true;
    for (size_t i = 0; i < list_size(body->decals); i++) {
        body_t *decal = list_get(body->decals, i);
        decal->destroy = true;
    }
}

bool body_is_removed(body_t *body) {
    return body->destroy;
}