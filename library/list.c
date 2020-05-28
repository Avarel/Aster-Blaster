#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <stdbool.h>

// TODO: list_clear() tests

const size_t GROWTH_FACTOR = 2;

typedef struct list {
    size_t size;
    size_t capacity;
    void (*freer)(void *ptr);
    void **data;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
    list_t *list = malloc(sizeof(list_t));
    assert(list != NULL);

    list->size = 0;
    list->capacity = initial_size;
    list->freer = freer;
    list->data = malloc(initial_size * sizeof(void*));
    assert(list->data != NULL);

    return list;
}

void list_free(list_t *list) {
    if (list->freer != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            list->freer(list->data[i]);
        }
    }
    free(list->data);
    free(list);
}

size_t list_size(list_t *list) {
    return list->size;
}

size_t list_capacity(list_t *list) {
    return list->capacity;
}

void *list_get(list_t *list, size_t index) {
    assert(index < list->size);
    return list->data[index];
}

/** PRIVATE METHOD
 * Resize the internal array of the list to a new and larger capacity.
 * 
 * @param list a pointer to a list returned from list_init()
 * @param new_capacity the new capacity of the internal array
 */
void list_expand(list_t *list, size_t new_capacity) {
    assert(new_capacity > list->capacity);
    list->data = realloc(list->data, new_capacity * sizeof(void *));
    assert(list->data != NULL);
    list->capacity = new_capacity;
}

void list_add(list_t *list, void *value) {
    assert(value != NULL);
    if (list->size >= list->capacity) {
        size_t new_cap = list->capacity == 0 ? 1 : GROWTH_FACTOR * list->capacity;
        list_expand(list, new_cap);
    }
    list->data[list->size] = value;
    list->size++;
}

void *list_remove(list_t *list, size_t index) {
    assert(list->size > 0);
    assert(index < list->size);
    void *removed = list->data[index];
    // copy the data n+1 to n
    memmove(list->data+index, list->data+index+1, (list->size - index - 1) * sizeof(void*));
    list->size--;
    return removed;
}

void *list_remove_item(list_t *list, void *ptr) {
    size_t len = list_size(list);
    for (size_t i = 0; i < len; i++) {
        if (list_get(list, i) == ptr) {
            return list_remove(list, i);
        }
    }
    return NULL;
}

void list_clear(list_t *list) {
    if (list->size > 0) {
        for (int i = list->size - 1; i >= 0; i--) { // i needs to be int
            void* value = list_remove(list, i);
            list->freer(value);
        }
    }
}

list_t *list_clone(list_t *list, clone_func_t copyr) {
    size_t len = list_size(list);
    list_t *new = list_init(len, list->freer);

    for (size_t i = 0; i < len; i++) {
        list_add(new, copyr(list_get(list, i)));
    }

    return new;
}

bool list_contains(list_t *list, void *ptr) {
    size_t len = list_size(list);
    
    for (size_t i = 0; i < len; i++) {
        if (list_get(list, i) == ptr) {
            return true;
        }
    }

    return false;
}