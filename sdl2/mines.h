#ifndef MINES_H
#define MINES_H

#include <stdbool.h>

extern struct game_board game_board;

enum GAME_STATE
{
  GAME_STATE_NEW,
  GAME_STATE_RUNNING,
  GAME_STATE_WIN,
  GAME_STATE_DEAD,
};

// int bombs_remaining;
// int time;

struct game_cell {
  int number_adjacent;
  bool is_open;
  bool is_mine;
  bool is_kaboom;
};

struct game_board
{
  int rows;
  int cols;
  int mines;
  struct game_cell *cells;
};

void game_init(int rows, int cols, int mines);
void game_destroy(void);

void cell_check(int row, int col);
void cell_uncover(int row, int col);

#endif // MINES_H