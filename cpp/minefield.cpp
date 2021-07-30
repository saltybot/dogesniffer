#include "minefield.h"

#include "shuffle.h"

Minefield::Minefield(int rows, int cols, int mines)
:Rows_(rows)
,Cols_(cols)
,Mines_(mines)
,Cells_()
{
    // if your provide invalid values, you're going to have a bad time
    if ((rows <= 0)
     || (cols <= 0)
     || (mines > ((rows * cols) - 1)))
    {
        Rows_ = 1;
        Cols_ = 1;
        Mines_ = 1;
    }

    // allocate minefield
    for (int row = 0; row < rows + 2; ++row)
    {
        std::vector<Cell> cellRow;
        for (int col = 0; col < cols + 2; ++col)
        {
            Cell cell;
            cellRow.push_back(cell);
        }
        Cells_.push_back(cellRow);
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
        Cells_[row][col].HasMine = true;
    }

    // calculate adjacent mines
    for (int row = 1; row <= Rows_; ++row)
    {
        for (int col = 1; col <= Cols_; ++col)
        {
            int numAdjacent = 0;
            numAdjacent += Cells[row - 1][col - 1].HasMine;
            numAdjacent += Cells[row - 1][col].HasMine;
            numAdjacent += Cells[row - 1][col + 1].HasMine;
            numAdjacent += Cells[row][col - 1].HasMine;
            numAdjacent += Cells[row][col + 1].HasMine;
            numAdjacent += Cells[row + 1][col - 1].HasMine;
            numAdjacent += Cells[row + 1][col].HasMine;
            numAdjacent += Cells[row + 1][col + 1].HasMine;
            Cells[row][col].MinesAdjacent = numAdjacent;
        }
    }
}

void Minefield::CheckCell(int row, int col)
{
    Cell &cell = Cells[row + 1][col + 1];
    
    if (!cell.HasFlag)
    {
        if (cell.IsOpen)
        {
            int flagsAdjacent = GetFlagsAdjacent(row, col);
            // if flags_adjacent == mines_adjacent (if flags_adjacent == 0, probably don't need DFS)
            if (flagsAdjacent == cell.MinesAdjacent)
            {
                // DFS
            }
        }
        else
        {
            cell.IsOpen = true;
            cell.IsKaboom = cell.HasMine;
            if (cell.MinesAdjacent == 0)
            {
                // DFS
            }
        }
    }
}

Minefield::Cell Minefield::GetCell(int row, int col) const
{
    return Cells[row + 1][col + 1];
}

bool Minefield::MoveMine(int srcRow, int srcCol, int destRow, int destCol)
{
    return false;
}

void Minefield::ToggleCellFlag(int row, int col)
{
    Cell &cell = Cells[row + 1][col + 1];
    cell.HasFlag = !cell.HasFlag;
}

int Minefield::GetFlagsAdjacent(int row, int col)
{
    return 0;
}