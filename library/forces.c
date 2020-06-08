#include "forces.h"
#include "body.h"
#include "collision.h"
#include "scene.h"
#include "utils.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define MIN_RADIUS_FOR_EFFECT 0.001

// NEWTONIAN GRAVITY

typedef struct newtonian_gravity_aux {
    double G;
    body_t *body1;
    body_t *body2;
    bool one_way;
} newtonian_gravity_aux_t;

void newtonian_gravity_handler(newtonian_gravity_aux_t *aux) {
    vector_t radius_vector = vec_subtract(body_get_centroid(aux->body2), body_get_centroid(aux->body1));
    double radius = vec_norm(radius_vector);

    double mass1 = body_get_mass(aux->body1);
    double mass2 = body_get_mass(aux->body2);
    vector_t force =
        radius >= MIN_RADIUS_FOR_EFFECT ? vec_multiply(-aux->G * mass1 * mass2 / (radius * radius), vec_normalize(radius_vector))
                   : VEC_ZERO;

    if (!aux->one_way) {
        body_add_force(aux->body2, force);
    }
    body_add_force(aux->body1, vec_negate(force));
}

void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2, bool one_way) {
    newtonian_gravity_aux_t *aux = malloc(sizeof(newtonian_gravity_aux_t));
    aux->G = G;
    aux->body1 = body1;
    aux->body2 = body2;
    list_t *list = list_init(2, NULL);
    list_add(list, body1);
    list_add(list, body2);
    aux->one_way = one_way;
    scene_add_bodies_force_creator(scene, (force_creator_t)newtonian_gravity_handler, aux, list, free);
}

// Attraction force

typedef struct attraction_aux {
    double A;
    body_t *body1;
    body_t *body2;
    bool one_way;
} attraction_aux_t;

void attraction_handler(attraction_aux_t *aux) {
    vector_t radius_vector = vec_subtract(body_get_centroid(aux->body2), body_get_centroid(aux->body1));
    double radius = vec_norm(radius_vector);

    vector_t force =
        radius >= MIN_RADIUS_FOR_EFFECT ? vec_multiply(-aux->A * radius, vec_normalize(radius_vector))
                   : VEC_ZERO;

    if (!aux->one_way) {
        body_set_velocity(aux->body2, force);
    }
    body_set_velocity(aux->body1, vec_negate(force));
}

void create_attraction(scene_t *scene, double A, body_t *body1, body_t *body2, bool one_way) {
    attraction_aux_t *aux = malloc(sizeof(attraction_aux_t));
    aux->A = A;
    aux->body1 = body1;
    aux->body2 = body2;
    list_t *list = list_init(2, NULL);
    list_add(list, body1);
    list_add(list, body2);
    aux->one_way = one_way;
    scene_add_bodies_force_creator(scene, (force_creator_t)attraction_handler, aux, list, free);
}

// Mirrored Attraction force

typedef struct attraction_mirrored_aux {
    double A;
    body_t *body_to_move;
    body_t *body_unaffected;
    vector_t sdl_max;
    vector_t offset;
} attraction_mirrored_aux_t;

void attraction_mirrored_handler(attraction_mirrored_aux_t *aux) {
    vector_t anti_center = vec_subtract(aux->sdl_max, body_get_centroid(aux->body_unaffected));
    anti_center = vec_add(anti_center, aux->offset);
    vector_t radius_vector = vec_subtract(anti_center, body_get_centroid(aux->body_to_move));
    double radius = vec_norm(radius_vector);

    vector_t force =
        radius >= MIN_RADIUS_FOR_EFFECT ? vec_multiply(-aux->A * radius, vec_normalize(radius_vector))
                   : VEC_ZERO;

    body_set_velocity(aux->body_to_move, vec_negate(force));
}

void create_attraction_mirrored(scene_t *scene, double A, body_t *body_to_move, body_t *body_unaffected, vector_t sdl_max, vector_t offset) {
    attraction_mirrored_aux_t *aux = malloc(sizeof(attraction_mirrored_aux_t));
    aux->A = A;
    aux->body_to_move = body_to_move;
    aux->body_unaffected = body_unaffected;
    list_t *list = list_init(2, NULL);
    list_add(list, body_to_move);
    list_add(list, body_unaffected);
    aux->sdl_max = sdl_max;
    aux->offset = offset;
    scene_add_bodies_force_creator(scene, (force_creator_t)attraction_mirrored_handler, aux, list, free);
}

// Pointing force

typedef struct pointing_force_aux {
    body_t *body_to_point;
    body_t *body_point_to;
} pointing_force_aux_t;

void pointing_force_handler(pointing_force_aux_t *aux) {
    vector_t to_point_pos = body_get_centroid(aux->body_to_point);
    vector_t point_to_pos = body_get_centroid(aux->body_point_to);

    bool right = to_point_pos.x > point_to_pos.x;

    if (to_point_pos.y == point_to_pos.y) { // prevent DBZ
        if (right) {
            body_set_rotation(aux->body_to_point, M_PI);
        } else {
            body_set_rotation(aux->body_to_point, 0);
        }
    } else {
        bool below = to_point_pos.y < point_to_pos.y;
        double theta = atan((to_point_pos.x - point_to_pos.x) / (point_to_pos.y - to_point_pos.y));
        theta += below ? 0.5 * M_PI : 1.5 * M_PI;
        body_set_rotation(aux->body_to_point, theta);
    }
}

void create_pointing_force(scene_t *scene, body_t *body_to_point, body_t *body_point_to) {
    pointing_force_aux_t *aux = malloc(sizeof(pointing_force_aux_t));
    aux->body_to_point = body_to_point;
    aux->body_point_to = body_point_to;
    list_t *list = list_init(2, NULL);
    list_add(list, body_to_point);
    list_add(list, body_point_to);
    scene_add_bodies_force_creator(scene, (force_creator_t)pointing_force_handler, aux, list, free);
}

// SPRING FORCE

typedef struct spring_aux {
    double k;
    body_t *body1;
    body_t *body2;
} spring_aux_t;

void spring_handler(spring_aux_t *aux) {
    vector_t radius_vector =
        vec_subtract(body_get_centroid(aux->body1), body_get_centroid(aux->body2));
    vector_t force = vec_multiply(-aux->k, radius_vector);
    body_add_force(aux->body1, force);
    body_add_force(aux->body2, vec_negate(force));
}

void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2) {
    spring_aux_t *aux = malloc(sizeof(spring_aux_t));
    aux->k = k;
    aux->body1 = body1;
    aux->body2 = body2;
    list_t *list = list_init(2, NULL);
    list_add(list, body1);
    list_add(list, body2);
    scene_add_bodies_force_creator(scene, (force_creator_t)spring_handler, aux, list, free);
}



// DRAG FORCE

typedef struct drag_aux {
    double gamma;
    body_t *body;
} drag_aux_t;

void drag_force_handler(drag_aux_t *aux) {
    body_add_force(aux->body, vec_negate(vec_multiply(aux->gamma, body_get_velocity(aux->body))));
}

void create_drag(scene_t *scene, double gamma, body_t *body) {
    drag_aux_t *aux = malloc(sizeof(drag_aux_t));
    aux->gamma = gamma;
    aux->body = body;
    list_t *list = list_init(1, NULL);
    list_add(list, body);
    scene_add_bodies_force_creator(scene, (force_creator_t)drag_force_handler, aux, list, free);
}



// COLLISION HANDLES

typedef struct collision_aux {
    body_t *body1; // BORROWED
    body_t *body2; // BORROWED
    collision_handler_t handler;
    void *aux;
    free_func_t freer;
} collision_aux_t;


void collision_aux_free(collision_aux_t *ptr) {
    if (ptr->freer != NULL & ptr->aux != NULL) {
        ptr->freer(ptr->aux);
    }
    free(ptr);
}

void collision_handle(collision_aux_t *aux) {
    const list_t *shape1 = body_borrow_shape(aux->body1);
    const list_t *shape2 = body_borrow_shape(aux->body2);

    collision_info_t info = find_collision(shape1, shape2);
    if (info.collided) {
            aux->handler(aux->body1, aux->body2, info.axis, aux->aux);
    }
}

void create_collision(
    scene_t *scene,
    body_t *body1,
    body_t *body2,
    collision_handler_t handler,
    void *aux,
    free_func_t freer) {
    collision_aux_t *caux = malloc(sizeof(collision_aux_t));
    caux->body1 = body1;
    caux->body2 = body2;
    caux->handler = handler;
    caux->aux = aux;
    caux->freer = freer;

    list_t *list = list_init(2, NULL);
    list_add(list, body1);
    list_add(list, body2);

    scene_add_bodies_force_creator(scene, (force_creator_t)collision_handle, caux, list, (free_func_t)collision_aux_free);
}





// MUTUALLY DESTRUCTIVE COLLISION

void destructive_collision_handler(body_t *body1, body_t *body2, vector_t axis, void *aux) {
    body_remove(body1);
    body_remove(body2);
}

void create_destructive_collision(scene_t *scene, body_t *body1, body_t *body2) {
    // destruction is simple, has no aux
    create_collision(scene, body1, body2, destructive_collision_handler, NULL, NULL);
}





// PHYSICAL COLLISION

typedef struct physics_collision_aux {
    double elasticity;
} physics_collision_aux;

void physics_collision_handler(body_t *body1, body_t *body2, vector_t axis, physics_collision_aux *aux) {
    double b1m = body_get_mass(body1);
    double b2m = body_get_mass(body2);

    double rm = isinf(b1m) || isinf(b2m) ? (isinf(b1m) ? (isinf(b2m) ? 0 : b2m) : b1m)  : b1m * b2m / (b1m + b2m);

    vector_t pos1 = body_get_centroid(body1);
    vector_t pos2 = body_get_centroid(body2);
    vector_t line = vec_subtract(pos2, pos1);
    if (vec_dot(line, axis) < 0.0) {
        axis = vec_negate(axis);
    }

    vector_t axis_norm = vec_normalize(axis);

    double ub = vec_dot(body_get_velocity(body2), axis_norm);
    double ua = vec_dot(body_get_velocity(body1), axis_norm);

    double factor = rm * (1 + aux->elasticity) * (ub - ua);
    vector_t impulse = vec_multiply(factor, axis_norm);

    // debugging information
    // printf("b1m: %f\nb2m: %f\nrm: %f\n", b1m, b2m, rm);
    // print_vec("axis:", axis_norm);
    // printf("ub - ua: %f - %f\n", ub, ua);

    // printf("factor: %f\n", factor);
    // print_vec("impulse:", impulse);

    // magical displacement factor?
    body_translate(body1, vec_multiply(1.0, vec_negate(axis)));

    body_add_impulse(body1, impulse);
    body_add_impulse(body2, vec_negate(impulse));
}

void create_physics_collision(
    scene_t *scene,
    double elasticity,
    body_t *body1,
    body_t *body2) {
    physics_collision_aux *aux = malloc(sizeof(physics_collision_aux));
    aux->elasticity = elasticity;
    create_collision(scene, body1, body2, (collision_handler_t)physics_collision_handler, aux, free);
}
