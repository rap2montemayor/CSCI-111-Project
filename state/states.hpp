#pragma once
#include <cassert>
#include <utility>
#include <vector>

enum tileState {
    HAS_GHOST = (1 << 0),
    HAS_PERSON = (1 << 1),
    HAS_LIGHT = (1 << 2)
};

struct boardState {
    std::vector<std::vector<int>> tileStates;
    long long int cost = 0;
    long long int MOVE_COST = 100;
    long long int LIGHTS_COST = 1;

    boardState();
    boardState(std::vector<std::vector<int>> tileStates);
    boardState(std::vector<std::vector<int>> tileStates, long long int cost);

    boardState copy();
    std::pair<int, int> getGhostPosition();
    std::pair<int, int> getPersonPosition();

    bool insideBoard(int row, int col);
    bool canMoveTo(int row, int col);
    bool canTurnOff(int row, int col);
    bool isWinState();

    boardState move(int drow, int dcol);
    boardState turnOff(int drow, int dcol);
    boardState moveUp();
    boardState moveLeft();
    boardState moveDown();
    boardState moveRight();
    boardState turnOffUp();
    boardState turnOffLeft();
    boardState turnOffDown();
    boardState turnOffRight();
};

struct candidateState {
    boardState board;
    std::vector<boardState> history;

    candidateState();
    candidateState(boardState board);
    candidateState(boardState board, std::vector<boardState> history);

    candidateState copy() ;
    candidateState moveUp();
    candidateState moveLeft();
    candidateState moveDown();
    candidateState moveRight();
    candidateState turnOffUp();
    candidateState turnOffLeft();
    candidateState turnOffDown() ;
    candidateState turnOffRight() ;
};
