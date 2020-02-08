#ifndef DOGESNIFFER_H
#define DOGESNIFFER_H

#include "SDL2/SDL.h"

#include <stdbool.h>

extern bool is_running;

void init_doge(SDL_Renderer *renderer, int rows, int cols, int mines);
void destroy_doge(void);
void process_input(void);
void update(void);
void render(void);

#endif // DOGESNIFFER_H