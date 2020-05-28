#include <stdlib.h>
#include <assert.h>
#include "vector.h"

typedef struct text_box {
    char *text;
    size_t font_size;
    vector_t origin;
    vector_t size;
} text_box_t;

text_box_t *text_box_init(char *text, size_t font_size, vector_t origin, vector_t size) {
    text_box_t *text_box = malloc(sizeof(text_box_t));
    assert(text_box != NULL);
    text_box->text = text;
    text_box->font_size = font_size;
    text_box->origin = origin;
    text_box->size = size;
    return text_box;
}

void text_box_free(text_box_t *text_box) {
    free(text_box->text);
    free(text_box);
}