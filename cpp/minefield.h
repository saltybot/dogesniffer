#pragma once

#include <vector>

class Minefield
{
public:
    struct Cell
    {
        int MinesAdjacent;
        bool IsOpen;
        bool HasFlag;
        bool HasMine;
        bool IsKaboom;
        
        Cell()
        :MinesAdjacent(0)
        ,IsOpen(false)
        ,HasFlag(false)
        ,HasMine(false)
        ,IsKaboom(false)
        { }
    };

    Minefield(int rows, int cols, int mines);
    ~Minefield();

    void CheckCell(int row, int col);
    Cell GetCell(int row, int col) const;
    int GetNumRows() const { return Rows; }
    int GetNumCols() const { return Cols; }
    int GetNumMines() const { return Mines; }
    bool MoveMine(int srcRow, int srcCol, int destRow, int destCol);
    void ToggleCellFlag(int row, int col);

private:
    int Rows_;
    int Cols_;
    int Mines_;
    std::vector<std::vector<Cell>> Cells_;

    int GetFlagsAdjacent(int row, int col);
};