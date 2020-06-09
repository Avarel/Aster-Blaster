#include "sdl_wrapper.h"
#include "text_box.h"
#include "utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

const char WINDOW_TITLE[] = "Aster Blaster";
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const double MS_PER_S = 1e3;
const char *FONT_PATH;

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

rgb_color_t background_color;

void sdl_set_background_color(rgb_color_t color) {
    background_color = color;
}

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void) {
    int *width = malloc(sizeof(*width)),
        *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center) {
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x,
           y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center) {
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)};
    return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key) {
    switch (key) {
    case SDLK_a:
    case SDLK_LEFT:
        return LEFT_ARROW;
    case SDLK_w:
    case SDLK_UP:
        return UP_ARROW;
    case SDLK_d:
    case SDLK_RIGHT:
        return RIGHT_ARROW;
    case SDLK_s:
    case SDLK_DOWN:
        return DOWN_ARROW;
    case SDLK_SPACE:
        return SPACE_BAR;
    case SDLK_ESCAPE:
        return ESCAPE;
    default:
        // Only process 7-bit ASCII characters
        return '\0';
        // return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

void sdl_init(vector_t min, vector_t max) {
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
}

void sdl_set_font(const char *font_path) {
    FONT_PATH = font_path;
}

bool sdl_is_done(void *aux) {
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);

    while (SDL_PollEvent(event)) {
        switch (event->type) {
        case SDL_QUIT: // TODO: do we need to call TTF_Quit() here? SDL_Quit() not called?
            free(event);
            return true;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            // Skip the keypress if no handler is configured
            // or an unrecognized key was pressed
            if (key_handler == NULL)
                break;
            char key = get_keycode(event->key.keysym.sym);
            if (key == '\0')
                break;

            uint32_t timestamp = event->key.timestamp;
            if (!event->key.repeat) {
                key_start_timestamp = timestamp;
            }
            key_event_type_t type =
                event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
            double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
            key_handler(key, type, held_time, aux);
            break;
        case SDL_MOUSEBUTTONDOWN:
            key_handler(SPACE_BAR, KEY_PRESSED, 0, aux);
            break;
        case SDL_MOUSEBUTTONUP:
            key_handler(SPACE_BAR, KEY_RELEASED, 0, aux);
            break;
        }
    }
    free(event);
    return false;
}

void sdl_clear(void) {
    rgb_color_t color = background_color;
    SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, color.a * 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(const body_t *body, vector_t window_center) {
    const list_t *points = body_borrow_shape(body);
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);

    render_info_t texture = body_get_render_data(body);
    // Draw polygon with the given color
    if (texture.type == COLOR) {
        rgb_color_t color = texture.data.color;
        assert(0 <= color.r && color.r <= 1);
        assert(0 <= color.g && color.g <= 1);
        assert(0 <= color.b && color.b <= 1);

        // Convert each vertex to a point on screen
        int16_t *x_points = malloc(sizeof(*x_points) * n),
                *y_points = malloc(sizeof(*y_points) * n);
        assert(x_points != NULL);
        assert(y_points != NULL);
        for (size_t i = 0; i < n; i++) {
            vector_t vertex = list_copy_vector(points, i);
            vector_t pixel = get_window_position(vertex, window_center);
            x_points[i] = pixel.x;
            y_points[i] = pixel.y;
        }

        filledPolygonRGBA(
            renderer,
            x_points, y_points, n,
            color.r * 255, color.g * 255, color.b * 255, 255);

        free(x_points);
        free(y_points);
    } else if (texture.type == TEX) {
        render_data_texture_t data = texture.data.texture;
        SDL_Texture *tex = data.tex;

        vector_t pos = get_window_position(body_get_centroid(body), window_center);
        int w = data.w;
        int h = data.h;
        int x = pos.x - data.dx;
        int y = pos.y - data.dy;
        double angle = body_get_angle(body);

        SDL_RenderCopyEx(renderer, tex, NULL, &(SDL_Rect){x, y, w, h}, -angle * 180 / M_PI, NULL, 0);
    }
}

void sdl_show(vector_t window_center) {
    // Draw boundary lines
    vector_t max = vec_add(center, max_diff),
             min = vec_subtract(center, max_diff);
    vector_t max_pixel = get_window_position(max, window_center),
             min_pixel = get_window_position(min, window_center);
    SDL_Rect *boundary = malloc(sizeof(*boundary));
    boundary->x = min_pixel.x;
    boundary->y = max_pixel.y;
    boundary->w = max_pixel.x - min_pixel.x;
    boundary->h = min_pixel.y - max_pixel.y;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, boundary);
    free(boundary);

    SDL_RenderPresent(renderer);
}

int sdl_render_text(const text_box_t *text_box) {
    TTF_Font *font = TTF_OpenFont(FONT_PATH, text_box_get_font_size(text_box));
    if (!font) {
        printf("Unable to load font: '%s'!\n"
               "SDL2_ttf Error: %s\n",
               FONT_PATH, TTF_GetError());
        return 0;
    }

    SDL_Color textColor = {0, 0, 0, 255};
    SDL_Color textBackgroundColor = {255, 255, 255, 255};
    SDL_Texture *text = NULL;
    SDL_Rect textRect;

    SDL_Surface *textSurface = TTF_RenderText_Shaded(font, text_box_borrow_text(text_box), textColor, textBackgroundColor);
    if (!textSurface) {
        printf("Unable to render text surface!\n"
               "SDL2_ttf Error: %s\n",
               TTF_GetError());
        TTF_CloseFont(font);
        return 0;
    }

    text = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!text) {
        printf("Unable to create texture from rendered text!\n"
               "SDL2 Error: %s\n",
               SDL_GetError());
        TTF_CloseFont(font);
        return 0;
    }

    textRect.w = textSurface->w;
    textRect.h = textSurface->h;

    SDL_FreeSurface(textSurface);

    switch (text_box_get_justification(text_box)) {
        case LEFT: {
            textRect.x = WINDOW_WIDTH - text_box_get_origin(text_box).x;
            break;
        }
        case RIGHT: {
            textRect.x = WINDOW_WIDTH - text_box_get_origin(text_box).x - textRect.w;
            break;
        }
        case CENTER: {
            textRect.x = WINDOW_WIDTH - text_box_get_origin(text_box).x - (textRect.w / 2.0);
            break;
        }
    }
    textRect.y = WINDOW_HEIGHT - text_box_get_origin(text_box).y;

    SDL_RenderCopy(renderer, text, NULL, &textRect);

    TTF_CloseFont(font);

    return 1;
}

void sdl_render_scene(const scene_t *scene) {
    sdl_clear();
    size_t body_count = scene_bodies(scene);
    vector_t window_center = get_window_center();
    for (size_t i = 0; i < body_count; i++) {
        const body_t *body = scene_borrow_body(scene, i);
        sdl_draw_polygon(body, window_center);
    }
    size_t text_box_count = scene_text_boxes(scene);
    for (size_t i = 0; i < text_box_count; i++) {
        sdl_render_text(scene_borrow_text_box(scene, i));
    }
    sdl_show(window_center);
}

void sdl_on_key(key_handler_t handler) {
    key_handler = handler;
}

SDL_Texture *sdl_load_texture(char *file) {
    return IMG_LoadTexture(renderer, file);
}

double time_since_last_tick(void) {
    clock_t now = clock();
    double difference = last_clock
                            ? (double)(now - last_clock) / CLOCKS_PER_SEC
                            : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}
