#pragma once

#include "minefield.h"

#include <vector>

class Minesweeper
{
public:
    enum GAME_STATE
    {
        GAME_STATE_NEW,
        GAME_STATE_RUNNING,
        GAME_STATE_WIN,
        GAME_STATE_DEAD,
        GAME_STATE_INVALID,
    };
/*
    struct Cell
    {
        //int FlagsAdjacent;
        int MinesAdjacent;
        bool IsOpen;
        bool HasFlag;
        bool HasMine;
        bool IsKaboom;
        
        Cell()
        //:FlagsAdjacent(0)
        //,MinesAdjacent(0)
        :MinesAdjacent(0)
        ,IsOpen(false)
        ,HasFlag(false)
        ,HasMine(false)
        ,IsKaboom(false)
        { }
    };

    struct Minefield
    {
        int Rows;
        int Cols;
        int Mines;
        std::vector<std::vector<Cell>> Cells;
    };
*/
    // render wants a collection of cells that have
    // - IsOpen
    // - HasFlag
    // - HasMine
    // - MinesAdjacent
    // - IsKaboom


    Minesweeper(int rows, int cols, int mines);
    
    // prevent copies
    Minesweeper(const Minesweeper&) = delete;
    Minesweeper& operator=(const Minesweeper&) = delete;
    
    Minesweeper(Minesweeper&&) = default;
    Minesweeper& operator=(Minesweeper&&) = default;

    ~Minesweeper();

    void CheckCell(int row, int col);
    void ToggleCellFlag(int row, int col);
    GAME_STATE GetGameState() const { return GameState; }
    const Minefield& GetMinefield() const { return Minefield; }

private:
    GAME_STATE GameState;
    Minefield Minefield;
};
