#include "render.h"

#include "mines.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <stdlib.h>

static const int CELL_WIDTH = 40;
static const int CELL_HEIGHT = 40;

static SDL_Window *gWindow = NULL;
static SDL_Renderer *gRenderer = NULL;
static SDL_Texture *gBlockTexture = NULL;
static SDL_Texture *gBombTexture = NULL;

void render_init(void)
{
  SDL_Init(SDL_INIT_VIDEO);
  SDL_InitSubSystem(SDL_INIT_VIDEO);

  SDL_VideoInit(NULL);
  SDL_Window *pWindow = SDL_CreateWindow(
      "dogesniffer",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      640, 480,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MOUSE_CAPTURE);

  gRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);

  gBlockTexture = IMG_LoadTexture(gRenderer, "block.png");
  gBombTexture = IMG_LoadTexture(gRenderer, "bomb.png");

  SDL_SetRenderDrawColor(gRenderer, 255, 240, 0, 255);
  SDL_RenderClear(gRenderer);
  SDL_RenderPresent(gRenderer);

  SDL_Rect rect = { 100, 100, 100, 100 };
  SDL_RenderFillRect(gRenderer, &rect);
}

void render_destroy(void)
{
    if (gBlockTexture != NULL)
  {
    SDL_DestroyTexture(gBlockTexture);
    gBlockTexture = NULL;
  }

  if (gBombTexture != NULL)
  {
    SDL_DestroyTexture(gBombTexture);
    gBombTexture = NULL;
  }

  if (gRenderer != NULL)
  {
    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
  }

  if (gWindow != NULL)
  {
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
  }

  SDL_QuitSubSystem(SDL_INIT_VIDEO);

  SDL_Quit();
}

void render(void)
{
  SDL_Rect rect = {0};
  rect.w = CELL_WIDTH;
  rect.h = CELL_HEIGHT;
  for (int i = 0; i < game_board.rows; ++i)
  {
    for (int j = 0; j < game_board.cols; ++j)
    {
      SDL_RenderCopy(gRenderer, gBlockTexture, NULL, &rect);
      rect.x += CELL_WIDTH;
    }
    rect.x = 0;
    rect.y += CELL_HEIGHT;
  }
  SDL_RenderPresent(gRenderer);
}