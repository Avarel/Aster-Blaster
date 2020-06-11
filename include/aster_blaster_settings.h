#ifndef __ASTER_BLASTER_SETTINGS__
#define __ASTER_BLASTER_SETTINGS__

#include "aster_blaster_imports.h"

// SDL settings
const vector_t SDL_MIN;
const vector_t SDL_MAX;
/**
 * Font designed by JoannaVu
 * Licensed for non-commercial use
 * Downloaded from fontspace.com/andromeda-font-f31762
 */
char FONT_PATH_ASTER_BLASTER[16];
// Menu settings
char MENU_TITLE_TEXT[14];
const size_t MENU_TITLE_FONT_SIZE;
#define MENU_TITLE_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.75 * SDL_MAX.y})
const justification_e MENU_TITLE_JUSTIFICATION;

char MENU_GAME_START_TEXT[22];
const size_t MENU_GAME_START_FONT_SIZE;
#define MENU_GAME_START_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.6 * SDL_MAX.y})
const justification_e MENU_GAME_START_JUSTIFICATION;

// Victory Screen settings
char VICTORY_TEXT[8];
const size_t VICTORY_FONT_SIZE;
#define VICTORY_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.75 * SDL_MAX.y})
const justification_e VICTORY_JUSTIFICATION;

char VICTORY_GAME_START_TEXT[22];
const size_t VICTORY_GAME_START_FONT_SIZE;
#define VICTORY_GAME_START_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.6 * SDL_MAX.y})
const justification_e VICTORY_GAME_START_JUSTIFICATION;

char VICTORY_MENU_TEXT[32];
const size_t VICTORY_MENU_FONT_SIZE;
#define VICTORY_MENU_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.5 * SDL_MAX.y})
const justification_e VICTORY_MENU_JUSTIFICATION;

//Global Settings
const double G;

// Player body settings
const double PLAYER_MASS;
#define PLAYER_INIT_POS ((vector_t){.x = SDL_MAX.x / 2, .y = 0.1 * SDL_MAX.y})
const double PLAYER_RADIUS;
const double PLAYER_SIDES;
const double PLAYER_SECTOR_SIDES;
const double PLAYER_ANGLE;
const rgb_color_t PLAYER_COLOR;
const double PLAYER_VELOCITY;
const double PLAYER_ACCELERATION;
const double PLAYER_SPACE_FRICTION;

// Player bullet settings
const double BULLET_MASS;
const vector_t BULLET_VELOCITY;
const double BULLET_RADIUS;
const double BULLET_SIDES;
const rgb_color_t BULLET_COLOR;
const double BULLET_COOLDOWN;

const double LASER_MASS;
const vector_t LASER_VELOCITY;
const rgb_color_t LASER_COLOR;
const double LASER_COOLDOWN;
const vector_t LASER_TRANSLATE;

const double MIN_MASS;

// Asteroid settings
const double ASTEROID_MIN_MASS;
const double ASTEROID_MAX_MASS;
const double ASTEROID_SPEED;
const double ASTEROID_RADIUS_MIN;
const double ASTEROID_RADIUS_MAX;
const rgb_color_t ASTEROID_COLOR;
const double ASTEROID_SPAWN_CHANCE;
const double ASTEROID_SPAWN_RATE;

// Background settings
const double NUM_STARS;
const double STAR_RADIUS_MIN;
const double STAR_RADIUS_MAX;
const double STAR_POINTS_MIN;
const double STAR_POINTS_MAX;
#define STAR_VELOCITY_1 ((vector_t){.x = 0, .y = -0.1 * SDL_MAX.y})
#define STAR_VELOCITY_2 ((vector_t){.x = 0, .y = -0.2 * SDL_MAX.y})
#define STAR_VELOCITY_3 ((vector_t){.x = 0, .y = -0.4 * SDL_MAX.y})
const rgb_color_t STAR_COLOR;

// Health bar settings
#define HEALTH_BAR_BACKGROUND_POS ((vector_t){.x = 0.0125 * SDL_MAX.y, .y = 0.0125 * SDL_MAX.y})
#define HEALTH_BAR_BACKGROUND_W (0.25 * SDL_MAX.x)
#define HEALTH_BAR_BACKGROUND_H (0.075 * SDL_MAX.y)
const rgb_color_t HEALTH_BAR_BACKGROUND_COLOR;
const size_t HEALTH_BAR_PADDING;
#define HEALTH_BAR_POS ((vector_t){.x = HEALTH_BAR_BACKGROUND_POS.x + HEALTH_BAR_PADDING, .y = HEALTH_BAR_BACKGROUND_POS.y + HEALTH_BAR_PADDING})
#define HEALTH_BAR_W (HEALTH_BAR_BACKGROUND_W - 2 * HEALTH_BAR_PADDING)
#define HEALTH_BAR_H (HEALTH_BAR_BACKGROUND_H - 2 * HEALTH_BAR_PADDING)
const rgb_color_t HEALTH_BAR_COLOR;

// Health settings
const double HEALTH_TOTAL;
const double DAMAGE_PER_MASS;

// Saw enemy settings
const double ENEMY_SAW_OUT_RADIUS;
const double ENEMY_SAW_IN_RADIUS;
const double ENEMY_SAW_POINTS;
const double ENEMY_SAW_MASS;
const rgb_color_t ENEMY_SAW_COLOR;
const double ENEMY_SAW_ELASTICITY;
const double ENEMY_SAW_OMEGA;
const double ENEMY_SAW_A;
const double ENEMY_SAW_SPAWN_RATE;
const size_t ENEMY_SAW_SWARM_SIZE_MIN;
const size_t ENEMY_SAW_SWARM_SIZE_MAX;

// Shooter enemy settings
const double ENEMY_SHOOTER_RADIUS;
const double ENEMY_SHOOTER_POINTS;
const double ENEMY_SHOOTER_SECTOR_POINTS;
const double ENEMY_SHOOTER_INIT_ANGLE;
const double ENEMY_SHOOTER_MASS;
const rgb_color_t ENEMY_SHOOTER_COLOR;
const double ENEMY_SHOOTER_A;
const double ENEMY_SHOOTER_SPAWN_RATE;

// Shooter enemy bullet settings
const double ENEMY_SHOOTER_SHOT_RATE;
const double ENEMY_SHOOTER_BULLET_OUT_RADIUS;
const double ENEMY_SHOOTER_BULLET_IN_RADIUS;
const size_t ENEMY_SHOOTER_BULLET_POINTS;
const double ENEMY_SHOOTER_BULLET_MASS;
const rgb_color_t ENEMY_SHOOTER_BULLET_COLOR;
const double ENEMY_SHOOTER_BULLET_SPEED;

// Boss settings
const double BOSS_OUT_RADIUS;
const double BOSS_IN_RADIUS;
const double BOSS_POINTS;
const double BOSS_MASS;
#define BOSS_INIT_POS ((vector_t){.x = SDL_MAX.x / 2, .y = 1.2 * SDL_MAX.y})
const double BOSS_HEALTH;
const rgb_color_t BOSS_COLOR;
const double BOSS_OMEGA;
const double BOSS_SPEED;
const double BOSS_SPAWN_TIME;

// Boss bullet settings
const double BOSS_SHOT_RATE;
const double BOSS_BOMB_RADIUS;
const double BOSS_BOMB_FUSE;
const double BOSS_BOMB_SPEED;
const size_t BOSS_BOMB_POINTS;
const rgb_color_t BOSS_BOMB_COLOR;
const size_t BOSS_BULLETS_PER_BOMB;
const double BOSS_BULLET_OUT_RADIUS;
const double BOSS_BULLET_IN_RADIUS;
const size_t BOSS_BULLET_POINTS;
const double BOSS_BULLET_MASS;
const rgb_color_t BOSS_BULLET_COLOR;
const double BOSS_BULLET_SPEED;

// Boss Health bar settings
#define BOSS_HEALTH_BAR_BACKGROUND_POS ((vector_t){.x = 0.0125 * SDL_MAX.y, .y = 0.925 * SDL_MAX.y})
#define BOSS_HEALTH_BAR_BACKGROUND_W (0.975 * SDL_MAX.x)
#define BOSS_HEALTH_BAR_BACKGROUND_H (0.075 * SDL_MAX.y)
const rgb_color_t BOSS_HEALTH_BAR_BACKGROUND_COLOR;
const size_t BOSS_HEALTH_BAR_PADDING;
#define BOSS_HEALTH_BAR_POS ((vector_t){.x = BOSS_HEALTH_BAR_BACKGROUND_POS.x + BOSS_HEALTH_BAR_PADDING, .y = BOSS_HEALTH_BAR_BACKGROUND_POS.y + BOSS_HEALTH_BAR_PADDING})
#define BOSS_HEALTH_BAR_W (BOSS_HEALTH_BAR_BACKGROUND_W - 2 * BOSS_HEALTH_BAR_PADDING)
#define BOSS_HEALTH_BAR_H (BOSS_HEALTH_BAR_BACKGROUND_H - 2 * BOSS_HEALTH_BAR_PADDING)
const rgb_color_t BOSS_HEALTH_BAR_COLOR;

// Black Hole settings
const double BLACK_HOLE_RADIUS;
const double BLACK_HOLE_POINTS;
const double BLACK_HOLE_MASS;
const double BLACK_HOLE_SPEED;
const rgb_color_t BLACK_HOLE_COLOR;
const double BLACK_HOLE_SPAWN_CHANCE;
const double BLACK_HOLE_SPAWN_RATE;

const double RATE_VARIANT_LOWER;
const double RATE_VARIANT_UPPER;
const int DEBUG_PRINT_RATE;

#endif // #ifndef __ASTER_BLASTER_SETTINGS__
