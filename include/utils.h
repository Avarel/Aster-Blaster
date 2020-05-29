#ifndef __UTILS__
#define __UTILS__

#include "color.h"
#include "vector.h"
#include "list.h"
#include <stdlib.h>
#include <stdbool.h>

/**
 * Macro to deflect objects off the edge.
 * Also kicks the polygon back to avoid
 * being stuck in a "negate loop" in the outer regions
 * 
 * _POL_: body object pointer
 * __P__: vector_t value/point
 * __Z__: component of vector_t
 * __E__: elasticity
 * _FVC_: vector component function
 * _MAX_: max bound
 * _MIN_: min bound
 */
#define EDGE_DEFLECT(_POL_, __P__, __Z__, __E__, _FVC_, _MIN_, _MAX_)       \
    double __Z__ = (__P__.__Z__);                                           \
    if ((__Z__) < (_MIN_.__Z__) || (__Z__) > (_MAX_.__Z__)) {               \
        vector_t temp_z = body_get_velocity(_POL_);                         \
        temp_z.__Z__ = -((__E__) * body_get_velocity(_POL_).__Z__);         \
        body_set_velocity(_POL_, temp_z);                                   \
        if ((__Z__) <= (_MIN_.__Z__)) {                                     \
            body_translate(_POL_, _FVC_((_MIN_.__Z__) - (__Z__)));          \
        } else {                                                            \
            body_translate(_POL_, _FVC_((_MAX_.__Z__) - (__Z__)));          \
        }                                                                   \
    }

vector_t *list_get_vector(list_t *list, size_t index);

/**
 * Prints vector info to the console for the demo.
 * 
 * @param label the label of the vector
 * @param vec the vector
 */
void print_vec(char label[], vector_t vec);

/**
 * Returns the a number modulo another number.
 * 
 * @param value The first argument
 * @param mod The second argument
 */
double modulo(double n, double m);

/**
 * Checks whether two doubles are within 1e-7
 * 
 * @param d1 the first double
 * @param d2 the second double
 * @return true if the doubles are within 1e-7, false otherwise
 */
bool is_close(double d1, double d2);
// again i feel like this is unnecessary

/**
 * Returns the maximum of two integers
 * 
 * @param n1 the first integer
 * @param n2 the second integer
 * @return the maximum of n1 and n2
 */
int imax(int n1, int n2);

/**
 * Returns the minimum of two integers
 * 
 * @param n1 the first integer
 * @param n2 the second integer
 * @return the minimum of n1 and n2
 */
int imin(int n1, int n2);

/**
 * Returns the maximum of two doubles
 * 
 * @param d1 the first double
 * @param d2 the second double
 * @return the maximum of d1 and d2
 */
int dmax(int d1, int d2);

/**
 * Returns the minimum of two doubles
 * 
 * @param d1 the first double
 * @param d2 the second double
 * @return the minimum of d1 and d2
 */
int dmin(int d1, int d2);


/**
 * Returns the number of digits in an integer
 * 
 * @param n the integer
 * @return the number of digits in n
 */
int dig_count(int n);

/**
 * Returns a random double in the range [min, max)
 * 
 * @param min the minimum value desired
 * @param max the maximum value desired, max must be > min
 * @return a random double in the range [min, max)
 */
double drand_range(double min, double max);

/**
 * Returns a random integer in the range [min, max]
 * 
 * @param min the minimum value desired
 * @param max the maximum value desired, max must be >= min
 * @return a random integer in the range [min, max]
 */
int irand_range(int min, int max);

/**
 * Returns a random color
 * 
 * @param min the min RGB value for each component
 * @param max the max RGB value for each component
 * @return a random color generated with given min and max
 */ 
rgb_color_t rand_color(double min, double max);

/**
 * Initializes the random seeds using current system time
 */ 
void init_random();

/**
 * Set the nth bit of a number to 1.
 */
size_t set_nth_bit(size_t num, size_t n);

/**
 * Set the nth bit of a number to 0.
 */
size_t clear_nth_bit(size_t num, size_t n);

/**
 * Get the nth bit of a number.
 */
size_t get_nth_bit(size_t num, size_t n);


#endif // #ifndef __UTILS__