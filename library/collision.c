#include "polygon.h"
#include "list.h"
#include "utils.h"
#include "vector.h"
#include <assert.h>
#include "collision.h"
#include <float.h>
#include <math.h>

// Rectangle box that approximates the max orthogonal rectangle
// that covers the polygon
typedef struct bounding_box {
    vector_t lower_left;
    vector_t upper_right;
} bounding_box_t;

bool bounding_box_collide(bounding_box_t box1, bounding_box_t box2) {
    // either box is not above or to the right of the other
    return !(box1.lower_left.y >= box2.upper_right.y ||
             box2.lower_left.y >= box1.upper_right.y) &&
           !(box1.lower_left.x >= box2.upper_right.x ||
             box2.lower_left.x >= box1.upper_right.x);
}

// Returns a bounding box
bounding_box_t polygon_bounding_box(list_t *poly) {
    assert(list_size(poly) > 0);
    vector_t point0 = *list_get_vector(poly, 0);
    bounding_box_t box = {.lower_left = point0, .upper_right = point0};

    for (size_t i = 1; i < list_size(poly); i++) {
        vector_t point = *list_get_vector(poly, i);
        if (point.x < box.lower_left.x) {
            box.lower_left.x = point.x;
        }
        if (point.x > box.upper_right.x) {
            box.upper_right.x = point.x;
        }
        if (point.y < box.lower_left.y) {
            box.lower_left.y = point.y;
        }
        if (point.y > box.upper_right.y) {
            box.upper_right.y = point.y;
        }
    }

    return box;
}

typedef struct interval {
    double low;
    double high;
} interval_t;

// contract: intervals are well formed (low <= high)
// returns the length of intersection (0 = no intersect)
double intervals_intersect(interval_t i1, interval_t i2) {
    if (i1.low <= i2.high && i2.low <= i1.high) {
        return fabs(fmin(i1.high, i2.high) - fmax(i1.low, i2.low));
    } else {
        return 0;
    }
}

// contract: axis is normalized (a unit vector)
interval_t polygon_project(list_t *poly, vector_t axis) {
    vector_t point0 = *list_get_vector(poly, 0);
    double point0d = vec_dot(point0, axis);
    interval_t interval = {.low = point0d, .high = point0d};
    
    // project onto the axis, giving the interval of the polygon
    // the interval is the length of the projected polygon on the axis
    for (size_t i = 1; i < list_size(poly); i++) {
        vector_t point = *list_get_vector(poly, i);
        double pointd = vec_dot(point, axis);
        if (pointd > interval.high) {
            interval.high = pointd;
        }
        if (pointd < interval.low) {
            interval.low = pointd;
        }
    }
    return interval;
}

// PRIVATE
typedef struct {
    bool collided;
    double intersect;
    vector_t axis;
} partial_sat_result_t;

// only perpendicular axes to the edges of poly1 are considered
// return true if a separating axis exists, returns the axis where there
// is the least intersection
partial_sat_result_t partial_sat(list_t *poly1, list_t *poly2) {
    vector_t min_axis = VEC_ZERO;
    double min_intersect = DBL_MAX;
    // employ separating axis theorem
    for (size_t i = 0; i < list_size(poly1); i++) {
        vector_t v1 = *list_get_vector(poly1, i);
        vector_t v2 = *list_get_vector(poly1, (i + 1) % list_size(poly1));

        // Get a parallel axis to the edge
        vector_t para_axis = vec_subtract(v2, v1);
        // Obtain a perpendicular axis to the edge
        vector_t perp_axis = vec_normalize(vec_perp(para_axis));

        // Project the polygon onto the axis as a line
        interval_t i1 = polygon_project(poly1, perp_axis);
        interval_t i2 = polygon_project(poly2, perp_axis);

        // Find if the lines intersect.
        double intersect = intervals_intersect(i1, i2);
        if (intersect == 0.0) {
            return (partial_sat_result_t){ .collided = false, .intersect = 0, .axis = VEC_ZERO };
        } else if (intersect < min_intersect) {
            min_intersect = intersect;
            min_axis = perp_axis;
        }
    }
    return (partial_sat_result_t){ .collided = true, .intersect = min_intersect, .axis = vec_multiply(min_intersect, min_axis) };
}

// perpendicular axes to edges of both polygons are considered
collision_info_t find_collision(list_t *poly1, list_t *poly2) {
   // make sure bounding boxes collide first
    if (!bounding_box_collide(polygon_bounding_box(poly1), polygon_bounding_box(poly2))) {
        return (collision_info_t){ .collided = false, .axis = VEC_ZERO };
    }

    partial_sat_result_t result1 = partial_sat(poly1, poly2);
    if (!result1.collided) {
        return (collision_info_t){ .collided = false, .axis = VEC_ZERO };
    }

    partial_sat_result_t result2 = partial_sat(poly2, poly1);
    if (!result2.collided) {
        return (collision_info_t){ .collided = false, .axis = VEC_ZERO };
    }
    
    // return the interval of lowest intersection
    if (result1.intersect < result2.intersect) {
        return (collision_info_t){ .collided = true, .axis = result1.axis };
    } else {
        return (collision_info_t){ .collided = true, .axis = vec_negate(result2.axis) };
    }
}
