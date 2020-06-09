#include "aster_blaster_imports.h"

void spawn_asteroid_top(scene_t *scene, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list) {
    // TODO: random later
    // TODO: magic number
    // TODO: split into spawn() and body_init() like everything else
    // TODO: image rendering should be factored out to sdl_wrapper
    double mass = drand_range(ASTEROID_MIN_MASS, ASTEROID_MAX_MASS);
    double ast_radius = (mass - ASTEROID_MIN_MASS) / (ASTEROID_MAX_MASS - ASTEROID_MIN_MASS) * (ASTEROID_RADIUS_MAX - ASTEROID_RADIUS_MIN) + ASTEROID_RADIUS_MIN;
    double ast_x = drand_range(SDL_MIN.x, SDL_MAX.x);
    vector_t ast_center = vec(ast_x, SDL_MAX.y + ast_radius);

    double theta = 0;
    double midpoint = (SDL_MAX.x - SDL_MIN.x) / 2;
    if (ast_x < midpoint) {
        theta = drand_range(3 * M_PI / 2, 2 * M_PI);
    } else {
        theta = drand_range(M_PI, 3 * M_PI / 2);
    }
    vector_t ast_velocity = vec(ASTEROID_SPEED * cos(theta), ASTEROID_SPEED * sin(theta));

    spawn_asteroid_general(scene, mass, ast_center, ast_velocity, bounds, ast_sprites_list);
}

void spawn_asteroid_general(scene_t *scene, double mass, vector_t ast_center, vector_t ast_velocity, game_bounds_t bounds, ast_sprites_list_t ast_sprites_list) {
    size_t num_sides;
    SDL_Texture *tex;

    switch (irand_range(0, 3)) {
    case 0:
        num_sides = 5;
        tex = ast_sprites_list.pentagon;
        break;
    case 1:
        num_sides = 6;
        tex = ast_sprites_list.hexagon;
    case 2:
        num_sides = 7;
        tex = ast_sprites_list.heptagon;
        break;
    case 3:
        num_sides = 10;
        tex = ast_sprites_list.circle;
        break;
    default:
        abort();
    }
    assert(tex != NULL);

    double ast_radius = (mass - ASTEROID_MIN_MASS) / (ASTEROID_MAX_MASS - ASTEROID_MIN_MASS) * (ASTEROID_RADIUS_MAX - ASTEROID_RADIUS_MIN) + ASTEROID_RADIUS_MIN;

    double ast_omega = drand_range(-2.0 * M_PI, 2.0 * M_PI);

    aster_aux_t *asteroid_aux = malloc(sizeof(aster_aux_t));
    asteroid_aux->body_type = ASTEROID;

    render_info_t texture = render_texture(tex, ast_radius * 2, ast_radius * 2);

    list_t *aster_shape = polygon_reg_ngon(ast_center, ast_radius, num_sides);
    body_t *asteroid = body_init_texture_with_info(aster_shape, mass, texture, asteroid_aux, free);
    body_set_velocity(asteroid, ast_velocity);
    body_set_omega(asteroid, ast_omega);

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_aster_bullet_collision(scene, asteroid, other_body, bounds, ast_sprites_list);
            } else if (other_aux->body_type == PLAYER) { // TODO: is this different from health collision?
                create_aster_player_collision(scene, asteroid, other_body);
            } else if (other_aux->body_type == BLACK_HOLE) {
                create_newtonian_gravity(scene, G, asteroid, other_body, true);
                create_destructive_collision_single(scene, asteroid, other_body);
            }
        }
    }

    destroy_at_bounds(scene, asteroid, bounds);
    scene_add_body(scene, asteroid);
}

/**
  * Creates all of the background stars and adds them to the scene. It is
  * important that this is called before any other bodies are added to the
  * scene so the stars go in the background.
  */
void create_background_stars(scene_t *scene, body_t *bound) {
    for (int i = 0; i < NUM_STARS; i++) {
        double r = drand_range(STAR_RADIUS_MIN, STAR_RADIUS_MAX);
        size_t degree = (size_t)irand_range(STAR_POINTS_MIN, STAR_POINTS_MAX);
        vector_t center = rand_vec(SDL_MIN, SDL_MAX);
        list_t *shape = polygon_reg_ngon(center, r, degree);
        // list_t *shape = polygon_star(center, r, r / 2, degree);
        body_t *star = body_init(shape, 0, STAR_COLOR);

        // Gives the star one of three velocities to create the illusion of
        // parallax.
        int which_velocity = irand_range(1, 3);
        switch (which_velocity) {
        case 1:
            body_set_velocity(star, STAR_VELOCITY_1);
            break;
        case 2:
            body_set_velocity(star, STAR_VELOCITY_2);
            break;
        case 3:
            body_set_velocity(star, STAR_VELOCITY_3);
            break;
        }
        create_star_collision(scene, star, bound);

        scene_add_body(scene, star);
    }
}

body_t *body_init_black_hole(vector_t pos, scene_t *scene, game_bounds_t bounds) {
    list_t *shape = polygon_reg_ngon(pos, BLACK_HOLE_RADIUS, BLACK_HOLE_POINTS);
    aster_aux_t *aster_aux = malloc(sizeof(aster_aux_t));
    aster_aux->body_type = BLACK_HOLE;
    body_t *black_hole = body_init_with_info(shape, BLACK_HOLE_MASS, BLACK_HOLE_COLOR, aster_aux, free);

    //if the black hole spawns at the left of the screen, x velocity should be
    //positive, so theta between 3*pi/2 and 2*pi
    //otherwise, theta between pi and 3*pi/2, so x velocity is negative
    double theta = 0;
    double midpoint = (SDL_MAX.x - SDL_MIN.x) / 2;
    if (body_get_centroid(black_hole).x < midpoint) {
        theta = drand_range(3 * M_PI / 2, 2 * M_PI);
    } else {
        theta = drand_range(M_PI, 3 * M_PI / 2);
    }
    vector_t bh_velocity = vec(BLACK_HOLE_SPEED * cos(theta), BLACK_HOLE_SPEED * sin(theta));
    body_set_velocity(black_hole, bh_velocity);

    for (size_t i = 0; i < scene_bodies(scene); i++) {
        body_t *other_body = scene_get_body(scene, i);
        aster_aux_t *other_aux = body_get_info(other_body);
        if (other_aux != NULL) {
            if (other_aux->body_type == BULLET) {
                create_newtonian_gravity(scene, G, other_body, black_hole, true);
                create_destructive_collision_single(scene, other_body, black_hole);
            } else if (other_aux->body_type != BLACK_HOLE && other_aux->body_type != PLAYER) {
                create_newtonian_gravity(scene, G, other_body, black_hole, true);
                create_destructive_collision_single(scene, other_body, black_hole);
            } else if (other_aux->body_type == PLAYER) {
                create_newtonian_gravity(scene, G, other_body, black_hole, true);
                create_collision(scene, black_hole, other_body, create_health_collision, NULL, NULL);
            }
        }
    }

    destroy_at_bounds(scene, black_hole, bounds);

    return black_hole;
}

void spawn_black_hole(scene_t *scene, game_bounds_t bounds) {
    double bh_x = drand_range(SDL_MIN.x, SDL_MAX.x);
    vector_t bh_center = vec(bh_x, SDL_MAX.y + BLACK_HOLE_RADIUS);
    body_t *black_hole = body_init_black_hole(bh_center, scene, bounds);
    scene_add_body(scene, black_hole);
}