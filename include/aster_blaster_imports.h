#ifndef __ASTER_BLASTER_IMPORTS__
#define __ASTER_BLASTER_IMPORTS__

// C lib
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
// high level
#include "scene.h"
#include "sdl_wrapper.h"
// mid level
#include "body.h"
#include "collision.h"
#include "forces.h"
#include "polygon.h"
#include "text_box.h"
// low level
#include "color.h"
#include "list.h"
#include "math.h"
#include "utils.h"
#include "vector.h"
// aster_blaster modules
#include "aster_blaster_settings.h"
#include "aster_blaster_typedefs.h"
#include "aster_blaster_enemies.h"
#include "aster_blaster_collisions.h"
#include "aster_blaster_environment.h"
#include "aster_blaster_player.h"
#include "aster_blaster_utils.h"
#include "aster_blaster_on_key.h"

// TODO: this should be factored out to sdl_wrapper
#include <SDL2/SDL_image.h>

#endif // #ifndef __ASTER_BLASTER_IMPORTS__