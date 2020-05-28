#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "body.h"
#include "forces.h"
#include "list.h"
#include "polygon.h"
#include "scene.h"
#include "test_util.h"
#include "vector.h"

// 1 Test for each force

const double CLOSE_ENOUGH = .01;

list_t *make_shape() {
    size_t initial = 4;
    list_t *shape = list_init(initial, free);
    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t){-1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){+1, -1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){+1, +1};
    list_add(shape, v);
    v = malloc(sizeof(*v));
    *v = (vector_t){-1, +1};
    list_add(shape, v);
    return shape;
}

bool close_to(vector_t vec1, vector_t vec2) {
    if ((vec1.x - CLOSE_ENOUGH < vec2.x) && (vec1.x + CLOSE_ENOUGH > vec2.x)) {
        return ((vec1.y - CLOSE_ENOUGH < vec2.y) && (vec1.y + CLOSE_ENOUGH > vec2.y));
    }
    return false;
}

void test_gravity() {
    double mass1 = 10;
    double mass2 = 20;
    double grav = 300;
    double timestep = 1e-6;
    int steps = 5000;
    scene_t *scene = scene_init();
    body_t *body1 = body_init(make_shape(), mass1, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, body1);
    body_set_centroid(body1, (vector_t){0, 0});
    body_t *body2 = body_init(make_shape(), mass2, (rgb_color_t){0, 0, 0});
    body_set_centroid(body2, (vector_t){50, 70});
    scene_add_body(scene, body2);
    create_newtonian_gravity(scene, grav, body1, body2);
    for (int i = 0; i < steps; i++) {
        scene_tick(scene, timestep);
        vector_t radius_vector =
            vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
        double radius = vec_norm(radius_vector);

        vector_t force =
            radius >= 0.001 ? vec_multiply(-grav * mass1 * mass2 / (radius * radius), vec_normalize(radius_vector))
                       : VEC_ZERO;

        double expected_x = body_get_centroid(body1).x + body_get_velocity(body1).x * timestep * i + -0.5 * force.x * timestep * timestep * i * i;
        double expected_y = body_get_centroid(body1).y + body_get_velocity(body1).y * timestep * i + -0.5 * force.y * timestep * timestep * i * i;
        assert(vec_within(1e-4, body_get_centroid(body1), vec(expected_x, expected_y)));
    }
    scene_free(scene);
}

void test_drag() {
    double mass1 = 10;
    double gamma = 0.5;
    double timestep = 1e-6;
    int steps = 100000;
    scene_t *scene = scene_init();
    body_t *body1 = body_init(make_shape(), mass1, (rgb_color_t){0, 0, 0});
    vector_t vel1 = (vector_t){100, 100};
    body_set_velocity(body1, vel1);
    scene_add_body(scene, body1);
    create_drag(scene, gamma, body1);
    vector_t prev_vel = body_get_velocity(body1);
    vector_t prev_cent = body_get_centroid(body1);
    scene_tick(scene, timestep);
    vector_t new_vel = body_get_velocity(body1);
    vector_t new_cent = body_get_centroid(body1);
    for (int i = 1; i < steps; i++) {
      vector_t acc = (vector_t) {-gamma * prev_vel.x/ mass1, -gamma * prev_vel.y/ mass1};
      assert(vec_isclose(new_vel,
                  (vector_t){prev_vel.x + acc.x * timestep, prev_vel.y + acc.y * timestep}));
      assert(vec_isclose(new_cent,
                  (vector_t){prev_cent.x + 0.5 * (prev_vel.x + new_vel.x) * timestep, prev_cent.y + 0.5 * (prev_vel.y + new_vel.y)* timestep}));
      scene_tick(scene, timestep);
      prev_vel = new_vel;
      prev_cent = new_cent;
      new_vel = body_get_velocity(body1);
      new_cent = body_get_centroid(body1);
    }
    scene_free(scene);
}

bool passed(double past, double current, double a) {
    return ((past < a && current > a) || (past > a && current < a));
}

void test_spring_period() {
    const double M = 2;    // mass
    const double K = 5000; // k constant
    const double A = 15;   // distance a
    const double DT = 1e-6;
    const int STEPS = 1000000;
    const double period = 2 * M_PI * sqrt(M / K);
    // creating scene variables
    scene_t *scene = scene_init();
    body_t *mass = body_init(make_shape(), M, (rgb_color_t){0, 0, 0});
    body_set_centroid(mass, (vector_t){A, 0});
    scene_add_body(scene, mass);
    body_t *anchor = body_init(make_shape(), INFINITY, (rgb_color_t){0, 0, 0});
    scene_add_body(scene, anchor);
    create_spring(scene, K, mass, anchor);
    int counter = 0;
    double time_counter = 0.0;
    double past = body_get_centroid(mass).x;
    double current;
    // running code
    for (int i = 0; i < STEPS; i++) {
        scene_tick(scene, DT);
        current = body_get_centroid(mass).x;
        if (passed(past, current, A)) {
            counter++;
            if (counter == 2) {
                assert(close_to((vector_t){time_counter, 0.0}, (vector_t){period, 0.0}));
                scene_free(scene);
                return;
            }
        }
        past = current;
        time_counter += DT;
    }
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_gravity)
    DO_TEST(test_drag)
    DO_TEST(test_spring_period)

    puts("student_tests PASS");
}
