#ifndef __TEXT_BOX_H__
#define __TEXT_BOX_H__

#include <stdlib.h>
#include "vector.h"

/**
 * A struct for rendering text with SDL.
 * Includes: text to render, font size, and position.
 */
typedef struct text_box text_box_t;

/**
 * Initializes a text box.
 * 
 * @param text the text
 * @param font_size the font size
 * @param origin the coordinate vector of the lower left corner of the text box
 */
text_box_t *text_box_init(char *text, size_t font_size, vector_t origin);

/**
 * Frees a text_box_t
 * 
 * @param text_box the text_box_t to free
 */
void text_box_free(text_box_t *text_box);

#endif // #ifndef __TEXT_BOX_H__