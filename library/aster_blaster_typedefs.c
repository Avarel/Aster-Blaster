#include "aster_blaster_imports.h"

void destroy_ast_sprite_list(ast_sprites_list_t list) {
    SDL_DestroyTexture(list.circle);
    SDL_DestroyTexture(list.heptagon);
    SDL_DestroyTexture(list.hexagon);
    SDL_DestroyTexture(list.pentagon);
    // SDL_DestroyTexture(list.ship);
}