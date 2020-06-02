#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "list.h"
#include "vector.h"
#include <stdbool.h>

/**
 * Computes the area of a polygon.
 * See https://en.wikipedia.org/wiki/Shoelace_formula#Statement.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the area of the polygon
 */
double polygon_area(const list_t *polygon);

/**
 * Computes the center of mass of a polygon.
 * See https://en.wikipedia.org/wiki/Centroid#Of_a_polygon.
 *
 * @param polygon the list of vertices that make up the polygon,
 * listed in a counterclockwise direction. There is an edge between
 * each pair of consecutive vertices, plus one between the first and last.
 * @return the centroid of the polygon
 */
vector_t polygon_centroid(const list_t *polygon);

/**
 * Translates all vertices in a polygon by a given vector.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param translation the vector to add to each vertex's position
 */
void polygon_translate(list_t *polygon, vector_t translation);

/**
 * Rotates vertices in a polygon by a given angle about a given point.
 * Note: mutates the original polygon.
 *
 * @param polygon the list of vertices that make up the polygon
 * @param angle the angle to rotate the polygon, in radians.
 * A positive angle means counterclockwise.
 * @param point the point to rotate around
 */
void polygon_rotate(list_t *polygon, double angle, vector_t point);

/**
 * Returns a pointer to a list_t that forms a rectangle.
 *
 * @param origin the point of the rectangle that w and h will span from
 * @param w the width of the rectangle
 * @param h the height of the rectangle
 * @return a pointer a list_t representing the rectangle
 */
list_t *polygon_rect(vector_t origin, double w, double h);

/**
 * Returns a pointer to a list_t that forms a regular n-sided polygon.
 *
 * @param center the center of the polygon
 * @param r the radius of the polygon (how far from the center the points are)
 * @param n the number of sides
 * @return a pointer a list_t representing the regular n-sided polygon
 */
list_t *polygon_reg_ngon(vector_t center, double r, size_t n);

/**
 * Returns a pointer to a list_t that forms a star polygon.
 * Note: a standard star will be formed with R > r > 0, but fun pseudo-stars
 *       can be formed by deviating from this constraint
 *
 * @param center the center of the star
 * @param R the outer radius of the star
 *          (how far from center the tips will be)
 * @param r the inner radius of the star
 *          (how far from center the concavities between arms will reach)
 * @param degree the number of arms the star will have, must be at least 2
 *          (although it won't really look star-like until degree 4 or 5)
 * @return a pointer a list_t representing the star
 */
list_t *polygon_star(vector_t center, double R, double r, size_t degree);

/**
 * Returns a pointer to a list_t that forms a sector of an n-gon.
 *
 * @param center the center of the sector
 * @param r the radius of the sector
 * @param N the sides of the n-gon, >= n + 2
 * @param n how many edges are removed to make the sector shape, >= 1
 * @param theta at what angle the middle of the sector cut should point
 * @return a pointer a list_t representing the sector
 */
list_t *polygon_ngon_sector(vector_t center, double r, size_t N, size_t n, double theta);

#endif // #ifndef __POLYGON_H__
