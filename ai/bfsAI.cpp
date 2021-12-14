#include "AI.hpp"

bfsAI::bfsAI(std::vector<std::vector<int>>& board): board(board) {
    reset();
}

void bfsAI::reset() {
    while (not bfsQueue.empty()) {
        bfsQueue.pop();
    }
    visitedStates.clear();
    cost = 0;
    stepsToSolution.clear();
    boardState initBoardState(board, 0);
    candidateState initCandidateState(initBoardState);
    bfsQueue.push(initCandidateState);
    goalStateMet = false;
}

void bfsAI::nextIteration() {
    if(goalStateMet) {
        board = goalStateReached.board.tileStates;
        return;
    } else if(bfsQueue.empty()) {
        //End state is unreachable
        //This should have an error flag
        return;
    } else {
        candidateState curr = bfsQueue.front(); bfsQueue.pop();
        board = curr.board.tileStates;
        cost = curr.board.cost;
        if(curr.board.isWinState()) {
            goalStateMet = true;
            goalStateReached = curr.copy();
        } else {
            std::vector<candidateState> nextStates;
            nextStates.push_back(curr.moveUp());
            nextStates.push_back(curr.moveLeft());
            nextStates.push_back(curr.moveDown());
            nextStates.push_back(curr.moveRight());
            nextStates.push_back(curr.turnOffUp());
            nextStates.push_back(curr.turnOffLeft());
            nextStates.push_back(curr.turnOffDown());
            nextStates.push_back(curr.turnOffRight());

            for (candidateState& i: nextStates) {
                if (visitedStates.count(i.board.tileStates) == 0) {
                    bfsQueue.push(i);
                    visitedStates.insert(i.board.tileStates);
                }
            }
        }
    }

    return;
}

bool bfsAI::isWinState() {
    return goalStateMet;
}

std::vector<std::vector<std::vector<int>>> bfsAI::getHistory() {
    if(isWinState()) {
        std::vector<std::vector<std::vector<int>>> historyAsGrid;
        for(boardState board: goalStateReached.history) {
            historyAsGrid.push_back(board.tileStates);
        }
        return historyAsGrid;
    } else {
        //Undefined behavior - win state not reached yet.
        std::vector<std::vector<std::vector<int>>> emptyVector;
        return emptyVector;
    }
}

