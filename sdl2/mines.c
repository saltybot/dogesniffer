#include "mines.h"
#include "util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct game_board game_board;
static bool game_is_started;

static struct game_cell* get_cell(int row, int col);
static int get_flags_adjacent(int row, int col);
static void move_mine(struct game_cell *cell);

void init_mines(int rows, int cols, int mines)
{
  destroy_mines();

  int num_cells = rows * cols;

  game_board.rows = rows;
  game_board.cols = cols;
  game_board.mines = mines;
  game_board.cells = malloc(sizeof(struct game_cell) * num_cells);
  memset(game_board.cells, 0, sizeof(struct game_cell) * num_cells);

  int *mah_list = malloc(num_cells * sizeof(int));
  for (int i = 0; i < num_cells; ++i)
  {
    mah_list[i] = i;
  }

  shuffle(mah_list, num_cells);
  for (int i = 0; i < mines; ++i)
  {
    printf("mah_list[%d] = %d\n",  i, mah_list[i]);
    game_board.cells[mah_list[i]].has_mine = true;
  }
  for (int i = 0; i < num_cells; ++i)
  {
    printf("cell: index=%d, has_mine=%d\n", i, game_board.cells[i].has_mine);
  }
  free(mah_list);

}

void destroy_mines(void)
{
  game_is_started = false;

  if (game_board.cells != NULL)
  {
    free(game_board.cells);
    game_board.cells = NULL;
  }
  memset(&game_board, 0, sizeof(game_board));
}

void check_cell(int row, int col)
{
  struct game_cell *cell = get_cell(row, col);

  if (!game_is_started)
  {
    if (cell->has_mine)
    {
      move_mine(cell);
    }
    game_is_started = true;
  }

  if (!cell->has_flag)
  {
    if (cell->is_open)
    {
      int flags_adjacent = get_flags_adjacent(row, col);
      // if flags_adjacent == mines_adjacent (if flags_adjacent == 0, probably don't need DFS)
      if (flags_adjacent == cell->mines_adjacent)
      {
        // DFS
      }
    }
    else
    {
      cell->is_open = true;
      cell->is_kaboom = cell->has_mine;
      if (cell->mines_adjacent == 0)
      {
        // DFS
      }
    }
  }
}

void toggle_cell_flag(int row, int col)
{
  struct game_cell *cell = get_cell(row, col);
  cell->has_flag = !cell->has_flag;
}


static struct game_cell* get_cell(int row, int col)
{
  return &game_board.cells[(row * 10) + col];
}

static int get_flags_adjacent(int row, int col)
{
  int min_row = row == 0 ? 0 : row - 1;
  int max_row = row == (game_board.rows - 1) ? game_board.rows - 1: row + 1;
  int min_col = col == 0 ? 0 : col - 1;
  int max_col = col == (game_board.cols - 1) ? game_board.cols - 1 : col + 1;

  int flags_adjacent = 0;
  for (int row = min_row; row <= max_row; ++row)
  {
    for (int col = min_col; col <= max_col; ++col)
    {
      struct game_cell *cell = get_cell(row, col);
      if (cell->has_flag)
      {
        ++flags_adjacent;
      }
    }
  }

  return flags_adjacent;
}

static void move_mine(struct game_cell *cell)
{
  if (cell->has_mine)
  {
    for (int i = 0; i < game_board.rows * game_board.cols; ++i)
    {
      if (!game_board.cells[i].has_mine)
      {
        game_board.cells[i].has_mine = true;
        cell->has_mine = false;
      }
    }
  }
}