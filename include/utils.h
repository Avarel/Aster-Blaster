#ifndef __UTILS__
#define __UTILS__

#include "color.h"
#include "vector.h"
#include "list.h"
#include <stdlib.h>
#include <stdbool.h>

double angle_to(vector_t to_point, vector_t point_to);

vector_t *list_get_vector(list_t *list, size_t index);

const vector_t *list_borrow_vector(const list_t *list, size_t index);

vector_t list_copy_vector(const list_t *list, size_t index);

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
 * Returns a random vector in the range between the two given vectors
 *
 * @param min the min vector
 * @param max the max vector
 * @return a random vector generated with given min and max
 */
vector_t rand_vec(vector_t min, vector_t max);

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
