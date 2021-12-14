#include "AI.hpp"

greedyAI::greedyAI(std::vector<std::vector<int>>& board): board(board) {
    reset();
}

void greedyAI::reset() {
    cost = 0;
    history.clear();
    boardState initBoardState(board, 0);
    history.push_back(initBoardState);
}

void greedyAI::nextIteration() {
    boardState currBoard(board, cost); //Insert initial board
    if(currBoard.isWinState() == false) {
        std::pair<int, int> ghostPosition = currBoard.getGhostPosition();
        std::pair<int, int> personPosition = currBoard.getPersonPosition();
        std::pair<int, int> direction = std::make_pair(personPosition.first - ghostPosition.first, personPosition.second - ghostPosition.second);
        if(direction.first != 0) {
            direction.first /= abs(direction.first);
        }
        if(direction.second != 0) {
            direction.second /= abs(direction.second);
        }
        std::pair<int, int> downTile = std::make_pair(ghostPosition.first + 1, ghostPosition.second);

        if(direction.first != 0 && currBoard.canMoveTo(ghostPosition.first + direction.first, ghostPosition.second)) {
            currBoard = currBoard.move(direction.first, 0);
            cost = currBoard.cost;
        } else if(direction.second != 0 && currBoard.canMoveTo(ghostPosition.first, ghostPosition.second + direction.second)) {
            currBoard = currBoard.move(0, direction.second);
            cost = currBoard.cost;
        } else if(direction.first != 0 && currBoard.canTurnOff(ghostPosition.first + direction.first, ghostPosition.second)) {
            currBoard = currBoard.turnOff(direction.first, 0);
            cost = currBoard.cost;
        } else if(direction.second != 0 && currBoard.canTurnOff(ghostPosition.first, ghostPosition.second + direction.second)) {
            currBoard = currBoard.turnOff(0, direction.second);
            cost = currBoard.cost;
        } else {
            assert(currBoard.isWinState());
        }
        history.push_back(currBoard.copy());
    }

    board = currBoard.tileStates;
    cost = currBoard.cost;
    return;
}

bool greedyAI::isWinState() {
    return history.back().isWinState();
}

std::vector<std::vector<std::vector<int>>> greedyAI::getHistory() {
    std::vector<std::vector<std::vector<int>>> historyAsGrid;
    for(boardState board: history) {
        historyAsGrid.push_back(board.tileStates);
    }
    return historyAsGrid;
}