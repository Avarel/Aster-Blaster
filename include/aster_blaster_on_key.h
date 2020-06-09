#ifndef __ASTER_BLASTER_ON_KEY__
#define __ASTER_BLASTER_ON_KEY__

#include "aster_blaster_imports.h"

void on_key_menu(char key, key_event_type_t type, double held_time, menu_keypress_aux_t *keypress_aux);

void on_key_victory(char key, key_event_type_t type, double held_time, menu_keypress_aux_t *keypress_aux);

void on_key_game(char key, key_event_type_t type, double held_time, game_keypress_aux_t *keypress_aux);

#endif // #ifndef __ASTER_BLASTER_ON_KEY__
