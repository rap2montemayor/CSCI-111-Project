#include "states.hpp"

boardState::boardState() { }

boardState::boardState(std::vector<std::vector<int>> tileStates) {
    this->tileStates = tileStates;
}

boardState::boardState(std::vector<std::vector<int>> tileStates, long long int cost) {
    this->tileStates = tileStates;
    this->cost = cost;
}

boardState boardState::copy() {
    boardState copiedState(tileStates, cost);
    return copiedState;
}

std::pair<int, int> boardState::getGhostPosition() {
    int row, col;
    for(row = 0; row < tileStates.size(); row++) {
        for(col = 0; col < tileStates[0].size(); col++) {
            if((tileStates[row][col] & (int)tileState::HAS_GHOST) == (int)tileState::HAS_GHOST) {
                return std::make_pair(row, col);
            }
        }
    }
    assert("Warning! Ghost not found" == "");
    return std::make_pair(row, col);
}

std::pair<int, int> boardState::getPersonPosition() {
    int row, col;
    for(row = 0; row < tileStates.size(); row++) {
        for(col = 0; col < tileStates[0].size(); col++) {
            if((tileStates[row][col] & (int)tileState::HAS_PERSON) == (int)tileState::HAS_PERSON) {
                return std::make_pair(row, col);
            }
        }
    }
    assert("Warning! Person not found" == "");
    return std::make_pair(row, col);
}

bool boardState::insideBoard(int row, int col) {
    bool valid = true;
    if(row < 0 || row >= tileStates.size()) {
        valid = false;
    }
    if(tileStates.empty() || col < 0 || col >= tileStates[0].size()) {
        valid = false;
    }
    return valid;
}

bool boardState::canMoveTo(int row, int col) {
    return insideBoard(row, col) && ((tileStates[row][col] & (int)tileState::HAS_LIGHT) == 0);
}

bool boardState::canTurnOff(int row, int col) {
    return insideBoard(row, col) && ((tileStates[row][col] & (int)tileState::HAS_LIGHT) == (int)tileState::HAS_LIGHT);
}

bool boardState::isWinState() {
    return getGhostPosition() == getPersonPosition();
}

boardState boardState::move(int drow, int dcol) {
    assert(abs(drow) + abs(dcol) == 1);
    boardState modifiedBoard = this->copy();
    std::pair<int, int> ghostPosition = getGhostPosition();
    int row = ghostPosition.first;
    int col = ghostPosition.second;

    if(canMoveTo(row+drow, col+dcol)) {
        modifiedBoard.tileStates[row][col] = modifiedBoard.tileStates[row][col] & (~(int)tileState::HAS_GHOST);
        modifiedBoard.tileStates[row+drow][col+dcol] = modifiedBoard.tileStates[row+drow][col+dcol] | (int)tileState::HAS_GHOST;
        modifiedBoard.cost += MOVE_COST;
    }
    return modifiedBoard;
}

boardState boardState::turnOff(int drow, int dcol) {
    assert(abs(drow) + abs(dcol) == 1);
    boardState modifiedBoard = this->copy();
    std::pair<int, int> ghostPosition = getGhostPosition();
    int row = ghostPosition.first;
    int col = ghostPosition.second;

    if(canTurnOff(row+drow, col+dcol)) {
        modifiedBoard.tileStates[row+drow][col+dcol] = modifiedBoard.tileStates[row+drow][col+dcol] & (int)(~(int)tileState::HAS_LIGHT);
        modifiedBoard.cost += LIGHTS_COST;
    }
    return modifiedBoard;
}

boardState boardState::moveUp() {
    return move(-1, 0);
}

boardState boardState::moveLeft() {
    return move(0, -1);
}

boardState boardState::moveDown() {
    return move(1, 0);
}

boardState boardState::moveRight() {
    return move(0, 1);
}

boardState boardState::turnOffUp() {
    return turnOff(-1, 0);
}

boardState boardState::turnOffLeft() {
    return turnOff(0, -1);
}

boardState boardState::turnOffDown() {
    return turnOff(1, 0);
}

boardState boardState::turnOffRight() {
    return turnOff(0, 1);
}
