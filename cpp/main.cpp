#include "minesweeper.h"
//#include "input.h"
//#include "mines.h"
//#include "render.h"

#include "SDL2/SDL.h"

static const int ROWS = 10;
static const int COLS = 10;
static const int MINES = 10;

static SDL_Window *window;
static SDL_Renderer *renderer;

static void InitGame();
static void DestroyGame();

const Uint32 FPS = 60;
const Uint32 FRAME_TIME = 1000 / FPS;

int main(int argc, char* argv[])
{
  InitGame();

  Minesweeper minesweeper(ROWS, COLS, MINES);

  bool isRunning = true;
  while (isRunning)
  {
    Uint32 frameStart = SDL_GetTicks();

    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_MOUSEBUTTONDOWN:
        {
          SDL_Point point = { .x = event.button.x, .y = event.button.y };
          //for (int i = 0; i < game_board.rows * game_board.cols; ++i)
          //{
            //SDL_Rect *rect = &game_board.cells[i].rect;
            //if (SDL_PointInRect(&point, rect))
            //{
              //SDL_SetRenderDrawColor(gRenderer, 255, 240, 0, 255);
              //SDL_RenderFillRect(gRenderer, rect);
              //SDL_RenderPresent(gRenderer);
              //break;
            //}
          break;
        }
        case SDL_QUIT:
        {
          isRunning = false;
          break;
        }
        default:
        {
          // do nothing
          break;
        }
      }
    }
    
    Uint32 elapsedTime = SDL_GetTicks() - frameStart;
    if (elapsedTime < FRAME_TIME)
    {
      SDL_Delay(FRAME_TIME - elapsedTime);
    }
  }

  DestroyGame();

  return 0;
}

static void InitGame()
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  SDL_VideoInit(NULL);
  window = SDL_CreateWindow(
    "dogesniffer",
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    640, 480,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MOUSE_CAPTURE);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  //init_mines(rows, cols, mines);
  //init_doge(renderer, rows, cols, mines);
}

static void DestroyGame()
{
  //destroy_doge();
  //destroy_mines();
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}
