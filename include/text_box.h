#ifndef __TEXT_BOX_H__
#define __TEXT_BOX_H__

#include <stdlib.h>
#include "vector.h"

typedef enum justification {
    LEFT,
    RIGHT,
    CENTER
} justification_e;

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
 * @param origin the coordinate vector of the origin
 *        (justification determines where it is placed relative to text)
 * @param justification how the text is position relative to origin
 * @return a text_box_t pointer
 */
text_box_t *text_box_init(char *text, size_t font_size, vector_t origin, justification_e justification);

/**
 * Doesn't free the text - it doesn't malloc it, so it doesn't free it 
 */
void text_box_free(text_box_t *text_box);

char *text_box_get_text(text_box_t *text_box);

const char *text_box_borrow_text(const text_box_t *text_box);

size_t text_box_get_font_size(const text_box_t *text_box);

vector_t text_box_get_origin(const text_box_t *text_box);

justification_e text_box_get_justification(const text_box_t *text_box);

#endif // #ifndef __TEXT_BOX_H__