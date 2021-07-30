#include "minesweeper.h"

#include "shuffle.h"

Minesweeper::Minesweeper(int rows, int cols, int mines)
:GameState(GAME_STATE_NEW)
,Minefield()
{
    // TODO don't allow an invalid object
    if (mines > ((rows * cols) - 1))
    {
        GameState = GAME_STATE_INVALID;
        return;
    }

    Minefield.Rows = rows;
    Minefield.Cols = cols;
    Minefield.Mines = mines;

    // allocate minefield
    for (int row = 0; row < rows + 2; ++row)
    {
        std::vector<Cell> cellRow;
        for (int col = 0; col < cols + 2; ++col)
        {
            Cell cell;
            cellRow.push_back(cell);
        }
        Minefield.Cells.push_back(cellRow);
    }

    // populate mines
    std::vector<int> mineList;
    for (int i = 0; i < rows * cols; ++i)
    {
        mineList.push_back(i);
    }
    shuffle(mineList);
    for (int i = 0; i < mines; ++i)
    {
        int mineIndex = mineList[i];
        int row = mineIndex / cols + 1;
        int col = mineIndex % cols + 1;
        Minefield.Cells[row][col].HasMine = true;
    }

    // calculate adjacent mines
    for (int row = 1; row <= Minefield.Rows; ++row)
    {
        for (int col = 1; col <= Minefield.Cols; ++col)
        {
            int numAdjacent = 0;
            numAdjacent += Minefield.Cells[row - 1][col - 1].HasMine;
            numAdjacent += Minefield.Cells[row - 1][col].HasMine;
            numAdjacent += Minefield.Cells[row - 1][col + 1].HasMine;
            numAdjacent += Minefield.Cells[row][col - 1].HasMine;
            numAdjacent += Minefield.Cells[row][col + 1].HasMine;
            numAdjacent += Minefield.Cells[row + 1][col - 1].HasMine;
            numAdjacent += Minefield.Cells[row + 1][col].HasMine;
            numAdjacent += Minefield.Cells[row + 1][col + 1].HasMine;
            Minefield.Cells[row][col].MinesAdjacent = numAdjacent;
        }
    }

    GameState = GAME_STATE_NEW;
}

Minesweeper::~Minesweeper()
{
    // do nothing
}

void Minesweeper::CheckCell(int row, int col)
{
  //struct game_cell *cell = get_cell(row, col);
  Cell &cell = Minefield.Cells[row][col];

  //if (!game_is_started)
  if (GameState == GAME_STATE_NEW)
  {
    // first click can't be a mine; move mine if necessary
    //if (cell->has_mine)
    if (cell.HasMine)
    {
      //for (int i = 0; i < game_board.rows * game_board.cols; ++i)
      for (int i = 0; i < Minefield.Rows * Minefield.Cols; ++i)
      {
        if (!game_board.cells[i].has_mine)
        {
          game_board.cells[i].has_mine = true;
          cell->has_mine = false;
        }
      }
    }
    GameState = GAME_STATE_RUNNING;
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

void Minesweeper::ToggleCellFlag(int row, int col)
{

}
