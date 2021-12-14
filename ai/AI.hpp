#pragma once
#include <queue>
#include <set>
#include <vector>
#include "../state/states.hpp"

class AI {
public:
    virtual void nextIteration() = 0;
    virtual bool isWinState() = 0;
    virtual std::vector<std::vector<std::vector<int>>> getHistory() = 0;
    virtual void reset() = 0;
};

class bfsAI: public AI {
    long long int cost = 0;

    bool goalStateMet;
    candidateState goalStateReached;
    std::queue<candidateState> bfsQueue;
    std::vector<boardState> stepsToSolution; 
    std::vector<std::vector<int>>& board;
    std::set<std::vector<std::vector<int>>> visitedStates;

public:
    bfsAI(std::vector<std::vector<int>>& board);
    void reset();
    void nextIteration();
    bool isWinState();
    std::vector<std::vector<std::vector<int>>> getHistory();
};

class greedyAI: public AI {
    long long int cost = 0;
    std::vector<boardState> history;
    std::vector<std::vector<int>>& board;

public:
    greedyAI(std::vector<std::vector<int>>& board);
    void reset();
    void nextIteration();
    bool isWinState();
    std::vector<std::vector<std::vector<int>>> getHistory();
};
