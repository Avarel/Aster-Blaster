#include "color.h"
#include "list.h"
#include "vector.h"
#include "body.h"
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double angle_to(vector_t to_point, vector_t point_to) {
    bool right = to_point.x > point_to.x;

    if (to_point.y == point_to.y) { // prevent DBZ
        return right ? M_PI : 0;
    } else {
        bool below = to_point.y < point_to.y;
        double theta = atan((to_point.x - point_to.x) / (point_to.y - to_point.y));
        theta += below ? 0.5 * M_PI : 1.5 * M_PI;
        return theta;
    }
}

vector_t *list_get_vector(list_t *list, size_t index) {
    return (vector_t *)list_get(list, index);
}

const vector_t *list_borrow_vector(const list_t *list, size_t index) {
    return (const vector_t *)list_borrow(list, index);
}

vector_t list_copy_vector(const list_t *list, size_t index) {
    return *list_borrow_vector(list, index);
}

double modulo(double n, double m) {
    double r = fmod(n, m);
    if (r < 0) {
        r += m;
    }
    return r;
}

bool is_close(double d1, double d2) {
    return fabs(d1 - d2) < 1e-7;
}

int imax(int n1, int n2) {
    return n1 > n2 ? n1 : n2;
}

int imin(int n1, int n2) {
    return n1 < n2 ? n1 : n2;
}

double drand_range(double min, double max) {
    assert(max > min);
    return drand48() * (max - min) + min;
}

int irand_range(int min, int max) {
    assert(max >= min);
    return (rand() % (max - min + 1)) + min;
}

rgb_color_t rand_color(double min, double max) {
    return rgb(drand_range(min, max),
               drand_range(min, max),
               drand_range(min, max));
}

vector_t rand_vec(vector_t min, vector_t max) {
    return vec(drand_range(min.x, max.x),
               drand_range(min.y, max.y));
}

size_t set_nth_bit(size_t num, size_t n) {
    return (1 << n) | num;
}

size_t clear_nth_bit(size_t num, size_t n) {
    return num & (~(1 << n));
}

size_t get_nth_bit(size_t num, size_t n) {
    return (num >> n) & 1;
}

void init_random() {
    srand(time(NULL));
    srand48(time(NULL));
}
