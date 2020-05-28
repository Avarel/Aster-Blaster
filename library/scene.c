#include "list.h"
#include "body.h"
#include "text_box.h"
#include "scene.h"
#include <assert.h>
#include <stdlib.h>

const size_t INITIAL_BODY_LIST_SIZE = 10;
const size_t INITIAL_FORCE_CREATOR_LIST_SIZE = 2;
const size_t INITIAL_TEXT_BOXES_LIST_SIZE = 1;

typedef struct force_creator_bundle {
    force_creator_t forcer;
    void *aux;
    list_t *bodies;
    free_func_t freer;
} force_creator_bundle_t;

typedef struct scene {
    list_t *bodies;
    list_t *force_creators;
    list_t *text_boxes;
} scene_t;

void force_creator_bundle_free(force_creator_bundle_t *bundle) {
    if (bundle->freer != NULL && bundle->aux != NULL) {
        bundle->freer(bundle->aux);
    }
    if (bundle->bodies != NULL) {
        list_free(bundle->bodies);
    }
    free(bundle);
}

scene_t *scene_init() {
    scene_t *scene = malloc(sizeof(scene_t));
    assert(scene != NULL);
    scene->bodies = list_init(INITIAL_BODY_LIST_SIZE, (free_func_t)body_free);
    scene->force_creators = list_init(INITIAL_FORCE_CREATOR_LIST_SIZE, (free_func_t)force_creator_bundle_free);
    scene->text_boxes = list_init(INITIAL_TEXT_BOXES_LIST_SIZE, (free_func_t)text_box_free);
    return scene;
}

void scene_free(scene_t *scene) {
    list_free(scene->bodies);
    list_free(scene->force_creators);
    list_free(scene->text_boxes);
    free(scene);
}

size_t scene_bodies(scene_t *scene) {
    return list_size(scene->bodies);
}

size_t scene_text_boxes(scene_t *scene) {
    return list_size(scene->text_boxes);
}

body_t *scene_get_body(scene_t *scene, size_t index) {
    return list_get(scene->bodies, index);
}

text_box_t *scene_get_text_box(scene_t *scene, size_t index) {
    return list_get(scene->text_boxes, index);
}

void scene_add_body(scene_t *scene, body_t *body) {
    list_add(scene->bodies, body);
}

void scene_add_text_box(scene_t *scene, text_box_t *text_box) {
    list_add(scene->text_boxes, text_box);
}

void scene_remove_body(scene_t *scene, size_t index) {
    body_remove(scene_get_body(scene, index));
}

void scene_remove_text_box(scene_t *scene, size_t index) {
    text_box_free(list_remove(scene->text_boxes, index));
}

force_creator_bundle_t *force_creator_bundle_init(force_creator_t forcer, void *aux, free_func_t freer, list_t *bodies) {
    force_creator_bundle_t *bundle = malloc(sizeof(force_creator_bundle_t));
    bundle->forcer = forcer;
    bundle->aux = aux;
    bundle->freer = freer;
    bundle->bodies = bodies;
    return bundle;
}

void scene_add_bodies_force_creator(
    scene_t *scene,
    force_creator_t forcer,
    void *aux,
    list_t *bodies,
    free_func_t freer
) {
    list_add(scene->force_creators, force_creator_bundle_init(forcer, aux, freer, bodies));
}

void scene_tick(scene_t *scene, double dt) {
    // If the force management is automatically done, then set the acceleration
    // to zero if there is no force creation.
    if (list_size(scene->force_creators) != 0) {
        for (size_t i = 0; i < scene_bodies(scene); i++) {
            body_set_acceleration(scene_get_body(scene, i), VEC_ZERO);
        }
    }
    for (size_t i = 0; i < list_size(scene->force_creators); i++) {
        force_creator_bundle_t* bundle = list_get(scene->force_creators, i);
        bundle->forcer(bundle->aux);
    }

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *body = scene_get_body(scene, i);

        // deferred body removal
        if (body_is_removed(body)) {
            body_free(list_remove(scene->bodies, i));

            // free the force bundle if it contains the removed body
            for (size_t i = 0; i < list_size(scene->force_creators); i++) {
                force_creator_bundle_t* bundle = list_get(scene->force_creators, i);
                if (bundle->bodies != NULL) {
                    if (list_contains(bundle->bodies, body)) {
                        force_creator_bundle_free(bundle);
                        list_remove(scene->force_creators, i);
                        i--;
                    }
                }

            }
        } else {
            body_tick(body, dt);
        }
    }

}
