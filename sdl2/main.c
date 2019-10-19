#include "dogesniffer.h"
#include "input.h"
#include "mines.h"
#include "render.h"

#include <SDL2/SDL.h>

#include <stdbool.h>

static const int ROWS = 10;
static const int COLS = 10;
static const int MINES = 10;

//static bool is_running = true;

//static const int CELL_WIDTH = 40;
//static const int CELL_HEIGHT = 40;
/*
struct cell {
  // rect should probably be refactored out the cell to decouple the graphics
  // from the game logic
  SDL_Rect rect;
  int number_adjacent;
  bool is_open;
  bool is_mine;
};

struct board
{
  int rows;
  int cols;
  int mines;
  struct cell cells[];
};

static SDL_Window *gWindow = NULL;
static SDL_Renderer *gRenderer = NULL;
static SDL_Texture *gBlockTexture = NULL;
static SDL_Texture *gBombTexture = NULL;

static struct board *gBoard = NULL;

void init_game(void)
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
}

void destroy_game(void)
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



// bombs can be list of indices that part of 'board', not 'cell'; or not :-)
// might make sense to make array of rects in 'board', so they can all be drawn in one pass; or not :-)

void load_textures(void)
{
//  gBlockTexture = IMG_LoadTexture(gRenderer, "block.png");
//  SDL_Surface *surface = SDL_LoadPNG("block.png");
//  blocktexture = SDL_CreateTextureFromSurface(surface);
}

void create_board(int rows, int cols, int mines, struct board **board)
{
  *board = malloc(sizeof(struct board) + ((rows * cols) * sizeof(struct cell)));

  if (board != NULL)
  {
    (*board)->rows = rows;
    (*board)->cols = cols;
    (*board)->mines = mines;

    int y = 0;
    for (int i = 0; i < rows; ++i)
    {
      int x = 0;
      for (int j = 0; j < cols; ++j)
      {
        struct cell *cell = &((*board)->cells[i * (*board)->rows + j]);
        cell->is_open = false;
        cell->is_mine = true;
        cell->number_adjacent = 0;
        cell->rect.x = x;
        cell->rect.y = y;
        cell->rect.w = CELL_WIDTH;
        cell->rect.h = CELL_HEIGHT;
        x += CELL_WIDTH;
      }
      y += CELL_HEIGHT;
    }
  }
}

void render_board(const struct board *board)
{
  SDL_Rect rect = {0};
  rect.w = CELL_WIDTH;
  rect.h = CELL_HEIGHT;
  for (int i = 0; i < board->rows; ++i)
  {
    for (int j = 0; j < board->cols; ++j)
    {
      SDL_RenderCopy(gRenderer, gBlockTexture, NULL, &rect);
      rect.x += CELL_WIDTH;
    }
    rect.x = 0;
    rect.y += CELL_HEIGHT;
  }
}
*/
/*
void render_rbg_board(SDL_Renderer *renderer, const struct board *board)
{
  int r = 20;
  int g = 60;
  int b = 40;
  for (int i = 0; i < board->rows; ++i)
  {
    for (int j = 0; j < board->cols; ++j)
    {
      const SDL_Rect *rect = &board->cells[i * board->rows + j].rect;
      SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
      SDL_RenderFillRect(renderer, rect);
      SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
      SDL_RenderDrawRect(renderer, rect);
      r += 4;
      g += 2;
      b += 1;
    }
    r += 10;
    g += 20;
    b += 40;
  }
}
*/
/*
void process_input_event(SDL_Event *event)
{
  switch (event->type)
  {
    case SDL_MOUSEBUTTONDOWN:
    {
      SDL_Point point = { .x = event->button.x, .y = event->button.y };
      for (int i = 0; i < gBoard->rows * gBoard->cols; ++i)
      {
        SDL_Rect *rect = &gBoard->cells[i].rect;
        if (SDL_PointInRect(&point, rect))
        {
          SDL_SetRenderDrawColor(gRenderer, 255, 240, 0, 255);
          SDL_RenderFillRect(gRenderer, rect);
          SDL_RenderPresent(gRenderer);
          break;
        }
      }
      break;
    }
  }
}
*/
const Uint32 FPS = 60;
const Uint32 FRAME_TIME = 1000 / FPS;

int main(int argc, char* argv[])
{
  game_init(ROWS, COLS, MINES);
  render_init();

  is_running = true;
  while (is_running)
  {
    Uint32 frame_start = SDL_GetTicks();

    process_input();
    render();

    Uint32 elapsed_time = SDL_GetTicks() - frame_start;
    if (elapsed_time < FRAME_TIME)
    {
      SDL_Delay(FRAME_TIME - elapsed_time);
    }
  }

  render_destroy();
  game_destroy();
/*
  init_game();

  // init textures
  SDL_Texture *close_cell = SDL_CreateTexture(
      gRenderer,
      SDL_PIXELFORMAT_RGB888,
      SDL_TEXTUREACCESS_STATIC,
      CELL_WIDTH,
      CELL_HEIGHT
  );
  SDL_Texture *bomb_cell = SDL_CreateTexture(
      gRenderer,
      SDL_PIXELFORMAT_RGB888,
      SDL_TEXTUREACCESS_STATIC,
      CELL_WIDTH,
      CELL_HEIGHT
  );
  SDL_Texture *number_cells[8];
  for (int i = 0; i < 8; ++i)
  {
    number_cells[i] = SDL_CreateTexture(
      gRenderer,
      SDL_PIXELFORMAT_RGB888,
      SDL_TEXTUREACCESS_STATIC,
      CELL_WIDTH,
      CELL_HEIGHT
    );
  }
  SDL_Texture *empty_cell = SDL_CreateTexture(
      gRenderer,
      SDL_PIXELFORMAT_RGB888,
      SDL_TEXTUREACCESS_STATIC,
      CELL_WIDTH,
      CELL_HEIGHT
  );

  SDL_SetRenderDrawColor(gRenderer, 255, 240, 0, 255);
  SDL_RenderClear(gRenderer);
  SDL_RenderPresent(gRenderer);

  SDL_Rect rect = { 100, 100, 100, 100 };
  SDL_RenderFillRect(gRenderer, &rect);

  game_init(ROWS, COLS, MINES);
  create_board(ROWS, COLS, MINES, &gBoard);
  render_board(gBoard);
  SDL_RenderPresent(gRenderer);
*/
/*
  while (1)
  {
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
      process_input_event(&event);

      if (event.type == SDL_QUIT)
      {
        break;
      }
    }
    //SDL_PumpEvents();
  }
*/

  //destroy_game();

  return 0;
}
