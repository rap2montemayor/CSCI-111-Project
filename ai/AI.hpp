#pragma once
#include <cassert>
#include <queue>
#include <vector>
#include <iostream>
#include <unordered_set>

class AI {
public:
    enum tileState {
        HAS_GHOST = (1 << 0),
        HAS_PERSON = (1 << 1),
        HAS_LIGHT = (1 << 2)
    };

    // AI(std::vector<std::vector<int>>& board) {

    // }; 
    virtual void nextIteration() = 0;

    virtual bool isWinState() = 0;

    virtual std::vector<std::vector<std::vector<int>>> getHistory() = 0;

    virtual void reset() = 0;



protected:
    std::vector<std::vector<int>> board;
    struct boardState {
        std::vector<std::vector<int>> tileStates;
        long long int cost = 0;
        long long int MOVE_COST = 100;
        long long int LIGHTS_COST = 1;

        boardState() {

        }

        boardState(std::vector<std::vector<int>> tileStates) {
            this->tileStates = tileStates;
        }

        boardState(std::vector<std::vector<int>> tileStates, long long int cost) {
            this->tileStates = tileStates;
            this->cost = cost;
        }

        boardState copy() {
            boardState copiedState(tileStates, cost);
            return copiedState;
        }

        std::pair<int, int> getGhostPosition() {
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

        std::pair<int, int> getPersonPosition() {
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

        bool insideBoard(int row, int col) {
            bool valid = true;
            if(row < 0 || row >= tileStates.size()) {
                valid = false;
            }
            if(tileStates.empty() || col < 0 || col >= tileStates[0].size()) {
                valid = false;
            }
            return valid;
        }

        bool canMoveTo(int row, int col) {
            return insideBoard(row, col) && ((tileStates[row][col] & (int)tileState::HAS_LIGHT) == 0);
        }

        bool canTurnOff(int row, int col) {
            return insideBoard(row, col) && ((tileStates[row][col] & (int)tileState::HAS_LIGHT) == (int)tileState::HAS_LIGHT);
        }

        bool isWinState() {
            return getGhostPosition() == getPersonPosition();
        }

        boardState move(int drow, int dcol) {
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
        
        boardState turnOff(int drow, int dcol) {
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

        boardState moveUp() {
            return move(-1, 0);
        }

        boardState moveLeft() {
            return move(0, -1);
        }

        boardState moveDown() {
            return move(1, 0);
        }

        boardState moveRight() {
            return move(0, 1);
        }

        boardState turnOffUp() {
            return turnOff(-1, 0);
        }

        boardState turnOffLeft() {
            return turnOff(0, -1);
        }

        boardState turnOffDown() {
            return turnOff(1, 0);
        }

        boardState turnOffRight() {
            return turnOff(0, 1);
        }
    };
};

class bfsAI: public AI {
    long long int cost = 0;

    struct candidateState {
        boardState board;

        std::vector<boardState> history;

        candidateState() {
            
        }

        candidateState(boardState board) {
            this->board = board.copy();
            history.push_back(board.copy());
        }

        candidateState(boardState board, std::vector<boardState> history) {
            this->board = board.copy();
            for(boardState state: history) {
                this->history.push_back(state.copy());
            }
        }

        candidateState copy() {
            candidateState copiedCandidateState(board.copy(), history);
            return copiedCandidateState;
        }

        candidateState moveUp() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveUp();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }

        candidateState moveLeft() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveLeft();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }

        candidateState moveDown() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveDown();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }

        candidateState moveRight() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveRight();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }

        candidateState turnOffUp() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffUp();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }

        candidateState turnOffLeft() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffLeft();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }

        candidateState turnOffDown() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffDown();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }

        candidateState turnOffRight() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffRight();
            nextCandidateState.history.push_back(nextCandidateState.board.copy());
            return nextCandidateState;
        }
    };
    bool goalStateMet;
    candidateState goalStateReached;
    std::queue<candidateState> bfsQueue;
    std::vector<boardState> stepsToSolution; 
    std::vector<std::vector<int>>& board;

public:
    bfsAI(std::vector<std::vector<int>>& board): board(board) {
        // this->board = board;
        reset();
        // boardState initBoardState(board, 0);
        // candidateState initCandidateState(initBoardState);
        // bfsQueue.push(initCandidateState);
        // goalStateMet = false;
    }

    void reset() {
        while (not bfsQueue.empty()) {
            bfsQueue.pop();
        }
        cost = 0;
        stepsToSolution.clear();
        boardState initBoardState(board, 0);
        candidateState initCandidateState(initBoardState);
        bfsQueue.push(initCandidateState);
        goalStateMet = false;
    }

    void nextIteration() {
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
                bfsQueue.push(curr.moveUp());
                bfsQueue.push(curr.moveLeft());
                bfsQueue.push(curr.moveDown());
                bfsQueue.push(curr.moveRight());
                bfsQueue.push(curr.turnOffUp());
                bfsQueue.push(curr.turnOffLeft());
                bfsQueue.push(curr.turnOffDown());
                bfsQueue.push(curr.turnOffRight());
            }
        }

        return;
    }

    bool isWinState() {
        return goalStateMet;
    }

    std::vector<std::vector<std::vector<int>>> getHistory() {
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
};

class greedyAI: public AI {
    long long int cost = 0;
    std::vector<boardState> history;
    std::vector<std::vector<int>>& board;

public:

    greedyAI(std::vector<std::vector<int>>& board): board(board) {
        // this->board = board;
        reset();
        // boardState initBoardState(board, 0);
        // history.push_back(initBoardState);
    }

    void reset() {
        cost = 0;
        history.clear();
        boardState initBoardState(board, 0);
        history.push_back(initBoardState);
    }

    void nextIteration() {
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

    bool isWinState() {
        return history.back().isWinState();
    }

    std::vector<std::vector<std::vector<int>>> getHistory() {
        std::vector<std::vector<std::vector<int>>> historyAsGrid;
        for(boardState board: history) {
            historyAsGrid.push_back(board.tileStates);
        }
        return historyAsGrid;
    }
};

/*int main() {
    int COL = 2;
    int ROW = 2;
    std::vector<std::vector<int>> v(ROW);
    for(int i = 0; i < ROW; i++) v[i].resize(COL);
    v[0][0] = 4; v[0][1] = 1;
    v[1][0] = 6; v[1][1] = 0;
    bfsAI bai(v);
    greedyAI gai(v);

    std::cout << "BFS TEST" << std::endl;
    while(!bai.isWinState()) {
        bai.nextIteration();
    }
    std::cout << "HISTORY BFS" << std::endl;
    for(std::vector<std::vector<int>> board: bai.getHistory()) {
        printBoardTiles(board);
        std::cout << std::endl;
    }

    std::cout << "GREEDY TEST" << std::endl;
    while(!gai.isWinState()) {
        gai.nextIteration();
    }
    std::cout << "HISTORY GREEDY" << std::endl;
    for(std::vector<std::vector<int>> board: gai.getHistory()) {
        printBoardTiles(board);
        std::cout << std::endl;
    }

    return 0;
}*/
