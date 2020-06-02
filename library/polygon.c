#include "polygon.h"
#include "list.h"
#include "math.h"
#include "utils.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>

/** PRIVATE METHOD
 * Computes the signed area of a polygon.
 * See https://en.wikipedia.org/wiki/Shoelace_formula#Statement.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the signed area of the polygon
 */
double polygon_shoelace(const list_t *polygon) {
    double shoelace = 0;
    int size = list_size(polygon);

    for (int i = 0; i < size; i++) {
        vector_t v1 = list_copy_vector(polygon, i);
        // Ensures last point can connect with first point.
        vector_t v2 = list_copy_vector(polygon, (i + 1) % size);

        shoelace += vec_cross(v1, v2);
    }

    shoelace *= 0.5;
    return shoelace;
}

double polygon_area(const list_t *polygon) {
    return fabs(polygon_shoelace(polygon));
}

vector_t polygon_centroid(const list_t *polygon) {
    double c_x = 0;
    double c_y = 0;

    size_t size = list_size(polygon);
    for (size_t i = 0; i < size; i++) {
        vector_t v1 = list_copy_vector(polygon, i);
        // Ensures last edge can connect with first edge.
        vector_t v2 = list_copy_vector(polygon, (i + 1) % size);

        double cross = vec_cross(v1, v2);
        c_x += (v1.x + v2.x) * cross;
        c_y += (v1.y + v2.y) * cross;
    }

    double area = polygon_shoelace(polygon);
    c_x /= 6 * area;
    c_y /= 6 * area;
    return (vector_t){.x = c_x, .y = c_y};
}

void polygon_translate(list_t *polygon, vector_t translation) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v = list_get_vector(polygon, i);
        *v = vec_add(*v, translation);
    }
}

void polygon_rotate(list_t *polygon, double angle, vector_t point) {
    for (size_t i = 0; i < list_size(polygon); i++) {
        vector_t *v = list_get_vector(polygon, i);
        // Get a vector depicting a point relative to the point of rotation.
        vector_t rel = vec_subtract(*v, point);
        // Rotate the vector.
        vector_t rot = vec_rotate(rel, angle);
        // Translate the vector so that it is now relative to the origin.
        vector_t new = vec_add(rot, point);
        *v = new;
    }
}

list_t *polygon_rect(vector_t origin, double w, double h) {
    list_t *rect = list_init(4, free);
    list_add(rect, vec_alloc(vec(origin.x, origin.y)));
    list_add(rect, vec_alloc(vec(origin.x + w, origin.y)));
    list_add(rect, vec_alloc(vec(origin.x + w, origin.y + h)));
    list_add(rect, vec_alloc(vec(origin.x, origin.y + h)));
    return rect;
}

list_t *polygon_reg_ngon(vector_t center, double r, size_t n) {
    list_t *ngon = list_init(n, free);
    for (size_t i = 0; i < n; i++) {
        vector_t point = vec(0, r);
        point = vec_rotate(point, i * (2 * M_PI) / n);
        point = vec_add(point, center);
        list_add(ngon, vec_alloc(point));
    }
    return ngon;
}

list_t *polygon_star(vector_t center, double R, double r, size_t degree) {
    assert(degree >= 2);
    list_t *star = list_init(2 * degree, free);
    for (size_t i = 0; i < 2 * degree; i += 2) {
        vector_t ext_point = vec(0, R);
        ext_point = vec_rotate(ext_point, i * M_PI / degree);
        ext_point = vec_add(ext_point, center);

        vector_t int_point = vec(0, r);
        int_point = vec_rotate(int_point, (i + 1) * M_PI / degree);
        int_point = vec_add(int_point, center);

        list_add(star, vec_alloc(ext_point));
        list_add(star, vec_alloc(int_point));
    }
    return star;
}

list_t *polygon_ngon_sector(vector_t center, double r, size_t N, size_t n, double theta) {
    assert(n >= 1);
    assert(N >= n + 2);
    list_t *ngon_sector = list_init(N - n + 2, free);
    list_add(ngon_sector, vec_alloc(center));
    for (size_t i = 0; i < N - n + 1; i++) {
        vector_t point = vec(0, r);
        point = vec_rotate(point, i * (2 * M_PI) / N);
        point = vec_add(point, center);
        list_add(ngon_sector, vec_alloc(point));
    }
    // the angle the sector cut is currently centered on
    double curr_theta = M_PI * (0.5 - ((double) n) / N);
    polygon_rotate(ngon_sector, theta - curr_theta, center);
    return ngon_sector; 
}