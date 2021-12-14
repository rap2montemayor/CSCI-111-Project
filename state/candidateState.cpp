#include "states.hpp"

candidateState::candidateState() { }

candidateState::candidateState(boardState board) {
    this->board = board.copy();
    history.push_back(board.copy());
}

candidateState::candidateState(boardState board, std::vector<boardState> history) {
    this->board = board.copy();
    for(boardState state: history) {
        this->history.push_back(state.copy());
    }
}

candidateState candidateState::copy() {
    candidateState copiedCandidateState(board.copy(), history);
    return copiedCandidateState;
}

candidateState candidateState::moveUp() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.moveUp();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}

candidateState candidateState::moveLeft() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.moveLeft();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}

candidateState candidateState::moveDown() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.moveDown();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}

candidateState candidateState::moveRight() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.moveRight();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}

candidateState candidateState::turnOffUp() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.turnOffUp();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}

candidateState candidateState::turnOffLeft() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.turnOffLeft();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}

candidateState candidateState::turnOffDown() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.turnOffDown();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}

candidateState candidateState::turnOffRight() {
    candidateState nextCandidateState = copy();
    nextCandidateState.board = nextCandidateState.board.turnOffRight();
    nextCandidateState.history.push_back(nextCandidateState.board.copy());
    return nextCandidateState;
}