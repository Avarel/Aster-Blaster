#include "aster_blaster_imports.h"

// SDL settings
const vector_t SDL_MIN = ((vector_t){.x = 0, .y = 0});
const vector_t SDL_MAX = ((vector_t){.x = 1200, .y = 800});
/**
 * Font designed by JoannaVu
 * Licensed for non-commercial use
 * Downloaded from fontspace.com/andromeda-font-f31762
 */
char FONT_PATH_ASTER_BLASTER[] = "./andromeda.ttf\0";
// Menu settings
char MENU_TITLE_TEXT[] = "Aster Blaster\0";
const size_t MENU_TITLE_FONT_SIZE = 64;
#define MENU_TITLE_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.75 * SDL_MAX.y})
const justification_e MENU_TITLE_JUSTIFICATION = CENTER;

char MENU_GAME_START_TEXT[] = "Press space to begin!\0";
const size_t MENU_GAME_START_FONT_SIZE = 32;
#define MENU_GAME_START_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.6 * SDL_MAX.y})
const justification_e MENU_GAME_START_JUSTIFICATION = CENTER;

// Victory Screen settings
char VICTORY_TEXT[] = "Victory\0";
const size_t VICTORY_SIZE = 64;
#define VICTORY_ORIGIN ((vector_t){.x = 0.5 * SDL_MAX.x, .y = 0.75 * SDL_MAX.y})
const justification_e VICTORY_JUSTIFICATION = CENTER;

// Global Settings
const double G = 1000;

// Player body settings
const double PLAYER_MASS = 100;
#define PLAYER_INIT_POS ((vector_t){.x = SDL_MAX.x / 2, .y = 0.1 * SDL_MAX.y})
const double PLAYER_RADIUS = 50;
const double PLAYER_SIDES = 15;
const double PLAYER_SECTOR_SIDES = 12;
const double PLAYER_ANGLE = (M_PI / 2);
const rgb_color_t PLAYER_COLOR = ((rgb_color_t){0.8, 0.3, 0.5});
const double PLAYER_VELOCITY = 1000;
const double PLAYER_ACCELERATION = 3000;
const double PLAYER_SPACE_FRICTION = 10.0;

// Player bullet settings
const double BULLET_MASS = 10;
#define BULLET_VELOCITY ((vector_t){.x = 0, .y = 2 * SDL_MAX.y})
const double BULLET_RADIUS = 12;
const double BULLET_SIDES = 30;
const rgb_color_t BULLET_COLOR = ((rgb_color_t){1, 1, 0});
const double BULLET_COOLDOWN = 0.2;

// Asteroid settings
const double ASTEROID_MIN_MASS = 50;
const double ASTEROID_MAX_MASS = 300;
const double ASTEROID_SIDES_MIN = 5;
const double ASTEROID_SIDES_MAX = 10;
const double ASTEROID_SPEED = 200;
const double ASTEROID_RADIUS_MIN = 30.0;
const double ASTEROID_RADIUS_MAX = 80.0;
const rgb_color_t ASTEROID_COLOR = (rgb_color_t){0.7, 0.7, 0.7};
const double ASTEROID_SPAWN_CHANCE = 0.33;
const double ASTEROID_SPAWN_RATE = 0.5;

// Background settings
const double NUM_STARS = 200;
const double STAR_RADIUS_MIN = 1;
const double STAR_RADIUS_MAX = 4;
const double STAR_POINTS_MIN = 4;
const double STAR_POINTS_MAX = 4;
#define STAR_VELOCITY_1 ((vector_t){.x = 0, .y = -0.1 * SDL_MAX.y})
#define STAR_VELOCITY_2 ((vector_t){.x = 0, .y = -0.2 * SDL_MAX.y})
#define STAR_VELOCITY_3 ((vector_t){.x = 0, .y = -0.4 * SDL_MAX.y})
const rgb_color_t STAR_COLOR = ((rgb_color_t){1.0, 1.0, 1.0});

// Health bar settings
#define HEALTH_BAR_BACKGROUND_POS ((vector_t){.x = 0.0125 * SDL_MAX.y, .y = 0.0125 * SDL_MAX.y})
#define HEALTH_BAR_BACKGROUND_W (0.25 * SDL_MAX.x)
#define HEALTH_BAR_BACKGROUND_H (0.075 * SDL_MAX.y)
const rgb_color_t HEALTH_BAR_BACKGROUND_COLOR = (rgb_color_t){1.0, 1.0, 1.0};
const size_t HEALTH_BAR_PADDING = 5;
#define HEALTH_BAR_POS ((vector_t){.x = HEALTH_BAR_BACKGROUND_POS.x + HEALTH_BAR_PADDING, .y = HEALTH_BAR_BACKGROUND_POS.y + HEALTH_BAR_PADDING})
#define HEALTH_BAR_W (HEALTH_BAR_BACKGROUND_W - 2 * HEALTH_BAR_PADDING)
#define HEALTH_BAR_H (HEALTH_BAR_BACKGROUND_H - 2 * HEALTH_BAR_PADDING)
const rgb_color_t HEALTH_BAR_COLOR = ((rgb_color_t){1.0, 0.25, 0.25});

// Health settings
const double HEALTH_TOTAL = 10000;
const double DAMAGE_PER_MASS = 1;

// Saw enemy settings
const double ENEMY_SAW_OUT_RADIUS = 25;
const double ENEMY_SAW_IN_RADIUS = 10;
const double ENEMY_SAW_POINTS = 8;
const double ENEMY_SAW_MASS = 80;
const rgb_color_t ENEMY_SAW_COLOR = (rgb_color_t){0.4, 1.0, 0.0};
const double ENEMY_SAW_ELASTICITY = 2;
const double ENEMY_SAW_OMEGA = 1.5 * M_PI;
const double ENEMY_SAW_A = 1;
const double ENEMY_SAW_SPAWN_RATE = 7;
const size_t ENEMY_SAW_SWARM_SIZE_MIN = 2;
const size_t ENEMY_SAW_SWARM_SIZE_MAX = 5;

// Shooter enemy settings
const double ENEMY_SHOOTER_RADIUS = 40;
const double ENEMY_SHOOTER_POINTS = 15;
const double ENEMY_SHOOTER_SECTOR_POINTS = 2;
const double ENEMY_SHOOTER_INIT_ANGLE = 0;
const double ENEMY_SHOOTER_MASS = 150;
const rgb_color_t ENEMY_SHOOTER_COLOR = (rgb_color_t){0.8, 0.8, 0.3};
const double ENEMY_SHOOTER_A = 0.65;
const double ENEMY_SHOOTER_SPAWN_RATE = 7;

// Shooter enemy bullet settings
const double ENEMY_SHOOTER_SHOT_RATE = 1;
const double ENEMY_SHOOTER_BULLET_OUT_RADIUS = 10;
const double ENEMY_SHOOTER_BULLET_IN_RADIUS = 5;
const size_t ENEMY_SHOOTER_BULLET_POINTS = 4;
const double ENEMY_SHOOTER_BULLET_MASS = 200;
const rgb_color_t ENEMY_SHOOTER_BULLET_COLOR = (rgb_color_t){0.8, 0.8, 0.3};
const double ENEMY_SHOOTER_BULLET_SPEED = 300;

// Boss settings
const double BOSS_OUT_RADIUS = 100;
const double BOSS_IN_RADIUS = 90;
const double BOSS_POINTS = 16;
const double BOSS_MASS = 100;
#define BOSS_INIT_POS ((vector_t){.x = SDL_MAX.x / 2, .y = 1.2 * SDL_MAX.y})
const double BOSS_HEALTH = 500;
const rgb_color_t BOSS_COLOR = (rgb_color_t){0.8, 0, 0};
const double BOSS_OMEGA = 1.5 * M_PI;
const double BOSS_SPEED = 200;
const double BOSS_SPAWN_TIME = 15;

// Boss attack settings
const double BOSS_SHOT_RATE = 7;
const double BOSS_BOMB_RADIUS = 30;
const double BOSS_BOMB_FUSE = 4;
const double BOSS_BOMB_SPEED = 200;
const rgb_color_t BOSS_BOMB_COLOR_1 = (rgb_color_t){0.376, 0.376, 0.376};
const rgb_color_t BOSS_BOMB_COLOR_2 = (rgb_color_t){1, 1, 0};
const rgb_color_t BOSS_BOMB_COLOR_3 = (rgb_color_t){1, 0.5, 0};
const rgb_color_t BOSS_BOMB_COLOR_4 = (rgb_color_t){1, 0, 0};
const size_t BOSS_BULLETS_PER_BOMB = 8;
const double BOSS_BULLET_OUT_RADIUS = 10;
const double BOSS_BULLET_IN_RADIUS = 5;
const size_t BOSS_BULLET_POINTS = 4;
const double BOSS_BULLET_MASS = 200;
const rgb_color_t BOSS_BULLET_COLOR = (rgb_color_t){0.8, 0.8, 0.3};
const double BOSS_BULLET_SPEED = 300;

// Boss Health bar settings
#define BOSS_HEALTH_BAR_BACKGROUND_POS ((vector_t){.x = 0.0125 * SDL_MAX.y, .y = 0.9875 * SDL_MAX.y})
#define BOSS_HEALTH_BAR_BACKGROUND_W (0.975 * SDL_MAX.x)
#define BOSS_HEALTH_BAR_BACKGROUND_H (0.075 * SDL_MAX.y)
const rgb_color_t BOSS_HEALTH_BAR_BACKGROUND_COLOR = (rgb_color_t){1.0, 1.0, 1.0};
const size_t BOSS_HEALTH_BAR_PADDING = 5;
#define BOSS_HEALTH_BAR_POS ((vector_t){.x = BOSS_HEALTH_BAR_BACKGROUND_POS.x + BOSS_HEALTH_BAR_PADDING, .y = BOSS_HEALTH_BAR_BACKGROUND_POS.y + BOSS_HEALTH_BAR_PADDING})
#define BOSS_HEALTH_BAR_W (BOSS_HEALTH_BAR_BACKGROUND_W - 2 * BOSS_HEALTH_BAR_PADDING)
#define BOSS_HEALTH_BAR_H (BOSS_HEALTH_BAR_BACKGROUND_H - 2 * BOSS_HEALTH_BAR_PADDING)
const rgb_color_t BOSS_HEALTH_BAR_COLOR = ((rgb_color_t){1.0, 0.25, 0.25});

// Black Hole settings
const double BLACK_HOLE_RADIUS = 50;
const double BLACK_HOLE_POINTS = 30;
const double BLACK_HOLE_MASS = 5e4;
const double BLACK_HOLE_SPEED = 100;
const rgb_color_t BLACK_HOLE_COLOR = (rgb_color_t){0.2, 0.2, 0.2};
const double BLACK_HOLE_SPAWN_CHANCE = 0.5;
const double BLACK_HOLE_SPAWN_RATE = 10;

const double RATE_VARIANT_LOWER = 0.8;
const double RATE_VARIANT_UPPER = 1.2;
const int DEBUG_PRINT_RATE = 200;
