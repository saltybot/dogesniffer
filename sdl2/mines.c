#include "mines.h"
#include "util.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct game_board game_board;

static void move_bomb(int row, int col);

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
    game_board.cells[mah_list[i]].is_mine = true;
  }
  for (int i = 0; i < num_cells; ++i)
  {
    printf("cell: index=%d, is_mine=%d\n", i, game_board.cells[i].is_mine);
  }
  free(mah_list);

}

void destroy_mines(void)
{
  if (game_board.cells != NULL)
  {
    free(game_board.cells);
    game_board.cells = NULL;
  }
  memset(&game_board, 0, sizeof(game_board));
}

void check_cell(int row, int col)
{

}

void uncover_cell(int row, int col)
{
  game_board.cells[(row * 10) + col].is_open = true;
}