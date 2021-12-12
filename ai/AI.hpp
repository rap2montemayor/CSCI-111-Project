#pragma once
#include <vector>

// To do

class AI {
    std::vector<std::vector<int>>& board; 

public:
    virtual AI(std::vector<std::vector<int>>& board) = 0;    
    virtual void nextIteration() = 0;
};