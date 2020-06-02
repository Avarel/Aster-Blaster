#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdbool.h>

/**
 * A real-valued 2-dimensional vector.
 * Positive x is towards the right; positive y is towards the top.
 * vector_t is defined here instead of vector.c because it is passed *by value*.
 */
typedef struct {
    double x;
    double y;
} vector_t;

/**
 * The zero vector, i.e. (0, 0).
 * "extern" declares this global variable without allocating memory for it.
 * You will need to define "const vector_t VEC_ZERO = ..." in vector.c.
 */
extern const vector_t VEC_ZERO;

/**
 * Returns a vector_t value made from the given arguments.
 * 
 * @param x the x component of the vector_t
 * @param y the y component of the vector_t
 * @return a vector_t value with the given components
 */
vector_t vec(double x, double y);

/**
 * Returns a vector_t value with just an x-component.
 * 
 * @param y the y component of the vector_t
 * @return a vector_t value (x, 0)
 */
vector_t vec_x(double x);

/**
 * Returns a vector_t value with just a y-component.
 * 
 * @param y the y component of the vector_t
 * @return a vector_t value (0, y)
 */
vector_t vec_y(double y);

/**
 * Returns a pointer to a vector_t value provided in the arguments.
 * 
 * @param v the vector_t value
 * @return a pointer to a vector_t with the given components
 */
vector_t *vec_alloc(vector_t v);

/**
 * Adds two vectors.
 * Performs the usual componentwise vector sum.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 + v2
 */
vector_t vec_add(vector_t v1, vector_t v2);

/**
 * Subtracts two vectors.
 * Performs the usual componentwise vector difference.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 - v2
 */
vector_t vec_subtract(vector_t v1, vector_t v2);

/**
 * Computes the additive inverse a vector.
 * This is equivalent to multiplying by -1.
 *
 * @param v the vector whose inverse to compute
 * @return -v
 */
vector_t vec_negate(vector_t v);

/**
 * Multiplies a vector by a scalar.
 * Performs the usual componentwise product.
 *
 * @param scalar the number to multiply the vector by
 * @param v the vector to scale
 * @return scalar * v
 */
vector_t vec_multiply(double scalar, vector_t v);

/**
 * Computes the dot product of two vectors.
 * See https://en.wikipedia.org/wiki/Dot_product#Algebraic_definition.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return v1 . v2
 */
double vec_dot(vector_t v1, vector_t v2);

/**
 * Computes the cross product of two vectors,
 * which lies along the z-axis.
 * See https://en.wikipedia.org/wiki/Cross_product#Computing_the_cross_product.
 *
 * @param v1 the first vector
 * @param v2 the second vector
 * @return the z-component of v1 x v2
 */
double vec_cross(vector_t v1, vector_t v2);

/**
 * Rotates a vector by an angle around (0, 0).
 * The angle is given in radians.
 * Positive angles are counterclockwise, according to the right hand rule.
 * See https://en.wikipedia.org/wiki/Rotation_matrix.
 * (You can derive this matrix by noticing that rotation by a fixed angle
 * is linear and then computing what it does to (1, 0) and (0, 1).)
 *
 * @param v the vector to rotate
 * @param angle the angle to rotate the vector
 * @return v rotated by the given angle
 */
vector_t vec_rotate(vector_t v, double angle);

/**
 * Returns a velocity vector after a given acceleration has been applied.
 * v(t) = v_0 + at
 * 
 * @param vel the velocity vector
 * @param acc the acceleration vector
 * @param t the duration for which the acceleration is applied
 * @return a vector representing the new velocity
 */
vector_t vec_tick(vector_t vel, vector_t acc, double t);

/**
 * Normalizes the vector so that its magnitude is 1.
 * 
 * @param vec the vector value
 * @return the normalized vector value
 */
vector_t vec_normalize(vector_t vec);

/**
 * Returns a perpendicular vector to the input vector
 * 
 * @param vec the vector value
 * @return the perpendicular vector value
 */
vector_t vec_perp(vector_t v);

/**
 * Returns the magnitude of the vector.
 * 
 * @param vec the vector value
 * @return the magnitude of vec
 */
double vec_norm(vector_t vec);

/**
 * Returns a clone of the supplied vector. The returned pointer has
 * the exact same value as the supplied vector.
 * 
 * @param vec the vector pointer
 * @return a pointer to an identical value to the supplied vector
 */
vector_t *vec_clone(const vector_t *vec);

#endif // #ifndef __VECTOR_H__
