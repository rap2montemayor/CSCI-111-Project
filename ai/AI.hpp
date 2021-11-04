#pragma once
#include <vector>

// To do

class AI {
    enum TileState {
        Passable = 0b000001,
        Dark     = 0b000010,
        HasGhost = 0b000100,
        HasChild = 0b001000,
        ToVisit  = 0b010000,
        Visited  = 0b100000
    };
    std::vector<std::vector<int>>& board;

public:
    virtual AI(std::vector<std::vector<int>>& board) = 0;    
    virtual vector<pair<int, int>> nextIteration() = 0;
};