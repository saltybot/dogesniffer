#include "input.h"

#include "dogesniffer.h"
#include "mines.h"

#include "SDL2/SDL.h"

void process_input(void)
{
  SDL_Event event;
  if (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_MOUSEBUTTONDOWN:
      {
        SDL_Point point = { .x = event.button.x, .y = event.button.y };
        for (int i = 0; i < game_board.rows * game_board.cols; ++i)
        {
          //SDL_Rect *rect = &game_board.cells[i].rect;
          //if (SDL_PointInRect(&point, rect))
          //{
            //SDL_SetRenderDrawColor(gRenderer, 255, 240, 0, 255);
            //SDL_RenderFillRect(gRenderer, rect);
            //SDL_RenderPresent(gRenderer);
            //break;
          //}
        }
        break;
      }
      case SDL_QUIT:
      {
        is_running = false;
        break;
      }
      default:
      {
        // do nothing
        break;
      }
    }
  }
}