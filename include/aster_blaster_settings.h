#ifndef __ASTER_BLASTER_SETTINGS__
#define __ASTER_BLASTER_SETTINGS__

// SDL settings
#define SDL_MIN ((vector_t){.x = 0, .y = 0})
#define SDL_MAX ((vector_t){.x = 1200, .y = 800})
/**
 * Font designed by JoannaVu
 * Licensed for non-commercial use
 * Downloaded from fontspace.com/andromeda-font-f31762
 */
char FONT_PATH_ASTER_BLASTER[] = "./andromeda.ttf\0";
// Menu settings
char MENU_TITLE_TEXT[] = "Aster Blaster\0";
#define MENU_TITLE_FONT_SIZE 64
#define MENU_TITLE_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.75 * SDL_MAX.y})
#define MENU_TITLE_JUSTIFICATION CENTER

char MENU_GAME_START_TEXT[] = "Press space to begin!\0";
#define MENU_GAME_START_FONT_SIZE 32
#define MENU_GAME_START_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.6 * SDL_MAX.y})
#define MENU_GAME_START_JUSTIFICATION CENTER

//Global Settings
#define G 50

// Player body settings
#define PLAYER_MASS 100
#define PLAYER_INIT_POS ((vector_t){.x = SDL_MAX.x / 2, .y = 0.1 * SDL_MAX.y})
#define PLAYER_RADIUS 50
#define PLAYER_SIDES 15
#define PLAYER_SECTOR_SIDES 12
#define PLAYER_ANGLE (M_PI / 2)
#define PLAYER_MASS 100
#define PLAYER_COLOR ((rgb_color_t){0.8, 0.3, 0.5})
#define PLAYER_VELOCITY 1000
#define PLAYER_ACCELERATION 3000
#define PLAYER_SPACE_FRICTION 10.0

// Bullet settings
#define BULLET_MASS 10
#define BULLET_VELOCITY ((vector_t){.x = 0, .y = 2 * SDL_MAX.y})
#define BULLET_RADIUS 12
#define BULLET_SIDES 30
#define BULLET_COLOR ((rgb_color_t){1, 1, 0})
#define BULLET_COOLDOWN 0.15

// Asteroid settings
#define ASTEROID_MASS 100
#define ASTEROID_SIDES_MIN 5
#define ASTEROID_SIDES_MAX 10
#define ASTEROID_SPEED 200
#define ASTEROID_RADIUS_MIN 30.0
#define ASTEROID_RADIUS_MAX 50.0
#define ASTEROID_COLOR ((rgb_color_t){0.7, 0.7, 0.7})
#define ASTEROID_SPAWN_CHANCE 0.33
#define ASTEROID_SPAWN_RATE 0.5

// Background settings
#define NUM_STARS 200
#define STAR_RADIUS_MIN 1
#define STAR_RADIUS_MAX 4
#define STAR_POINTS_MIN 4
#define STAR_POINTS_MAX 4
#define STAR_VELOCITY_1 ((vector_t){.x = 0, .y = -0.1 * SDL_MAX.y})
#define STAR_VELOCITY_2 ((vector_t){.x = 0, .y = -0.2 * SDL_MAX.y})
#define STAR_VELOCITY_3 ((vector_t){.x = 0, .y = -0.4 * SDL_MAX.y})
#define STAR_COLOR ((rgb_color_t){1.0, 1.0, 1.0})

// Health bar settings
#define HEALTH_BAR_BACKGROUND_POS ((vector_t){.x = 0.0125 * SDL_MAX.y, .y = 0.0125 * SDL_MAX.y})
#define HEALTH_BAR_BACKGROUND_W (0.25 * SDL_MAX.x)
#define HEALTH_BAR_BACKGROUND_H (0.075 * SDL_MAX.y)
#define HEALTH_BAR_BACKGROUND_COLOR ((rgb_color_t){1.0, 1.0, 1.0})
#define HEALTH_BAR_PADDING 5
#define HEALTH_BAR_POS ((vector_t){.x = HEALTH_BAR_BACKGROUND_POS.x + HEALTH_BAR_PADDING, .y = HEALTH_BAR_BACKGROUND_POS.y + HEALTH_BAR_PADDING})
#define HEALTH_BAR_W (HEALTH_BAR_BACKGROUND_W - 2 * HEALTH_BAR_PADDING)
#define HEALTH_BAR_H (HEALTH_BAR_BACKGROUND_H - 2 * HEALTH_BAR_PADDING)
#define HEALTH_BAR_COLOR ((rgb_color_t){1.0, 0.25, 0.25})

// Health settings
#define HEALTH_TOTAL 1000
#define DAMAGE_PER_MASS 1

// Saw enemy settings
#define ENEMY_SAW_OUT_RADIUS 25
#define ENEMY_SAW_IN_RADIUS 10
#define ENEMY_SAW_POINTS 8
#define ENEMY_SAW_MASS 80
#define ENEMY_SAW_COLOR ((rgb_color_t){0.4, 1.0, 0.0})
#define ENEMY_SAW_ELASTICITY 2
#define ENEMY_SAW_OMEGA (1.5 * M_PI)
#define ENEMY_SAW_A 1

// Black Hole settings
#define BLACK_HOLE_OUT_RADIUS 50
#define BLACK_HOLE_IN_RADIUS 45
#define BLACK_HOLE_POINTS 30
#define BLACK_HOLE_MASS 500
#define BLACK_HOLE_SPEED 100
#define BLACK_HOLE_IN_COLOR ((rgb_color_t){0, 0, 0})
#define BLACK_HOLE_OUT_COLOR ((rgb_color_t){0.8, 0.4, 0})
#define BLACK_HOLE_SPAWN_CHANCE 0.33
#define BLACK_HOLE_SPAWN_RATE 0.05


#define DEBUG_PRINT_RATE 200

#endif // #ifndef __ASTER_BLASTER_SETTINGS__
