#include "dogesniffer.h"
#include "mines.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <stdbool.h>

bool is_running = false;

static const int CELL_WIDTH = 40;
static const int CELL_HEIGHT = 40;

static bool game_running = false;
static int game_start_time = 0;

static int rowz;
static int colz;
static int minez;

static SDL_Renderer *Renderer = NULL;
static SDL_Texture *block_texture = NULL;
static SDL_Texture *bomb_texture = NULL;
static SDL_Texture *flagged_bomb_texture = NULL;
static SDL_Texture *kaboom_texture = NULL;
static SDL_Texture *number_textures[9] = {0};

static bool mouse_left_click;
static bool mouse_right_click;
static int mouse_x;
static int mouse_y;

struct cell
{
  SDL_Rect rect;
  SDL_Texture *texture;
};
static struct cell *cellz;

void init_doge(SDL_Renderer *renderer, int rows, int cols, int mines)
{
  destroy_doge();

  Renderer = renderer;
  rowz = rows;
  colz = cols;
  minez = mines;

  block_texture = IMG_LoadTexture(renderer, "block.png");
  bomb_texture = IMG_LoadTexture(renderer, "bomb.png");
  kaboom_texture = IMG_LoadTexture(renderer, "kaboom.png");
  number_textures[0] = IMG_LoadTexture(renderer, "0.png");
  number_textures[1] = IMG_LoadTexture(renderer, "1.png");
  number_textures[2] = IMG_LoadTexture(renderer, "2.png");
  number_textures[3] = IMG_LoadTexture(renderer, "3.png");
  number_textures[4] = IMG_LoadTexture(renderer, "4.png");
  number_textures[5] = IMG_LoadTexture(renderer, "5.png");
  number_textures[6] = IMG_LoadTexture(renderer, "6.png");
  number_textures[7] = IMG_LoadTexture(renderer, "7.png");
  number_textures[8] = IMG_LoadTexture(renderer, "8.png");

  cellz = malloc(sizeof(struct cell) * rows * cols);
  int cell_index = 0;
  int y = 0;
  for (int row = 0; row < rows; ++row)
  {
    int x = 0;
    for (int col = 0; col < cols; ++col)
    {
      struct cell *cell = &cellz[cell_index];
      cell->rect.x = x;
      cell->rect.y = y;
      cell->rect.w = CELL_WIDTH;
      cell->rect.h = CELL_HEIGHT;
      cell->texture = block_texture;

      x += CELL_WIDTH;
      ++cell_index;
    }
    y += CELL_HEIGHT;
  }
}

void destroy_doge(void)
{
  if (cellz != NULL)
  {
    free(cellz);
    cellz = NULL;
  }
}

void process_input(void)
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
      case SDL_MOUSEBUTTONDOWN:
      {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          mouse_left_click = true;
          mouse_x = event.button.x;
          mouse_y = event.button.y;
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
          mouse_right_click = true;
          mouse_x = event.button.x;
          mouse_y = event.button.y;
        }
        //SDL_Point point = { .x = event.button.x, .y = event.button.y };
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
        //}
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

void update(void)
{
  if (mouse_left_click || mouse_right_click)
  {
    SDL_Point point = { .x = mouse_x, .y = mouse_y };
    int cell_index = 0;
    for (int row = 0; row < rowz; ++row)
    {
      for (int col = 0; col < colz; ++col)
      {
        if (SDL_PointInRect(&point, &cellz[cell_index].rect))
        {
          if (mouse_left_click) {
            check_cell(row, col);
          }
          if (mouse_right_click) {
            toggle_cell_flag(row, col);
          }
        }
        ++cell_index;
      }
    }

    cell_index = 0;
    for (int row = 0; row < rowz; ++row)
    {
      for (int col = 0; col < colz; ++col)
      {
        struct game_cell *cell = &game_board.cells[cell_index];
        if (cell->is_open)
        {
          if (cell->is_kaboom)
          {
            cellz[cell_index].texture = kaboom_texture;
          }
          else if (cell->has_mine)
          {
            cellz[cell_index].texture = bomb_texture;
          }
          else
          {
            cellz[cell_index].texture = number_textures[cell->mines_adjacent];
          }
        }
        else
        {
          cellz[cell_index].texture = block_texture;
        }
        ++cell_index;
      }
    }

    mouse_left_click = false;
    mouse_right_click = false;
    mouse_x = 0;
    mouse_y = 0;
  }

  SDL_GetTicks();
}

void render(void)
{
  //for (int i = 0; i < rows * cols; ++i)
  for (int i = 0; i < 100; ++i)
  {
    SDL_RenderCopy(Renderer, cellz[i].texture, NULL, &cellz[i].rect);
  }
  SDL_RenderPresent(Renderer);
}