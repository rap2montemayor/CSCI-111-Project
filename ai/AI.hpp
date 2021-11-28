#pragma once
#include <vector>
#include <stack>
#include <iostream>
#include <algorithm>
#include <list>
#include <iostream>

// To do

class AI {
public:
    enum TileState {
        Passable = 0b000001,
        Dark     = 0b000010,
        HasGhost = 0b000100,
        HasChild = 0b001000,
        ToVisit  = 0b010000,
        Visited  = 0b100000
    };
    virtual std::vector<std::pair<int, int>> nextIteration() = 0;
    virtual std::vector<std::pair<int, int>> nextSolutionStep() = 0;
    virtual void constructSolution() = 0;
    virtual bool isGoalState() = 0;
    virtual void reset() = 0;
};

class DFS : public AI {
    int solutionstep;
    std::pair<int, int> ghost;
    std::stack<std::pair<int, int>> toVisit;
    std::vector<std::vector<int>> &board;
    std::vector<std::pair<int, int>> solution;

    bool valid(int r, int c);
    bool visited(int r, int c);
    bool passable(int r, int c);

public:
    DFS(std::vector<std::vector<int>>& board);
    std::vector<std::pair<int, int>> nextIteration();
    std::vector<std::pair<int, int>> nextSolutionStep();
    void constructSolution();
    bool isGoalState();
    void reset();
};