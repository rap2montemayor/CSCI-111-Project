#pragma once
#include <cassert>
#include <queue>
#include <vector>

class AI {
public:
    AI(std::vector<std::vector<int>>& board) {

    }; 
    std::pair<int, int> nextIteration();

protected:
    std::vector<std::vector<int>>& board = *(new std::vector<std::vector<int>>);
    struct boardState {
        enum class tileState: int {
            HAS_GHOST = (1 << 0),
            HAS_PERSON = (1 << 1),
            HAS_LIGHT = (1 << 2)
        };

        std::vector<std::vector<int>> tileStates;
        long long int cost = 0;
        long long int MOVE_COST = 1;
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
                    if((tileStates[row][col] & (int)tileState::HAS_GHOST) == true) {
                        return std::make_pair(row, col);
                    }
                }
            }
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
            for(std::vector<int> tileRow: tileStates) {
                for(int tile: tileRow) {
                    if(isTileWinState(tile)) {
                        return true;
                    }
                }
            }
            return false;
        }

        bool isTileWinState(int tile) {
            return (tile & (int)tileState::HAS_GHOST) && (tile & (int)tileState::HAS_PERSON);
        }

        boardState move(int drow, int dcol) {
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

        bool initialActionSet;
        boardState ancestralState; //The first state after the current state that eventually leads to this candidate state
        std::pair<int, int> ancestralModification; //The tile that differed between the current state and ancestralState

        candidateState() {
            
        }

        candidateState(boardState board) {
            this->board = board.copy();
            initialActionSet = false;
        }

        candidateState copy() {
            candidateState copiedCandidateState(board.copy());
            copiedCandidateState.initialActionSet = initialActionSet;
            copiedCandidateState.ancestralState = ancestralState.copy();
            copiedCandidateState.ancestralModification = ancestralModification;
            return copiedCandidateState;
        }

        /**
         * Indicates that this is the first state after the current state. 
         */
        void populateAncestralState(candidateState &state) {
            state.ancestralState = state.board.copy();
            state.ancestralModification = std::make_pair(0, 0);
            state.initialActionSet = true;
            for(int row = 0; row < board.tileStates.size(); row++) {
                for(int col = 0; col < board.tileStates[0].size(); col++) {
                    if(board.tileStates[row][col] != state.board.tileStates[row][col]) {
                        state.ancestralModification = std::make_pair(row, col);
                    }
                }
            }
        }

        candidateState moveUp() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveUp();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }

        candidateState moveLeft() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveLeft();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }

        candidateState moveDown() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveDown();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }

        candidateState moveRight() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.moveRight();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }

        candidateState turnOffUp() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffUp();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }

        candidateState turnOffLeft() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffLeft();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }

        candidateState turnOffDown() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffDown();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }

        candidateState turnOffRight() {
            candidateState nextCandidateState = copy();
            nextCandidateState.board = nextCandidateState.board.turnOffRight();
            if(nextCandidateState.initialActionSet == false) {
                populateAncestralState(nextCandidateState);
            }
            return nextCandidateState;
        }
    };


public:
    bfsAI(std::vector<std::vector<int>>& board): AI(board) {
        this->board = board;
    }

    std::pair<int, int> nextIteration() {
        boardState currBoard(board, cost); //Insert initial board 
        boardState nextBoard = currBoard.copy(); //The next state of the board
        std::pair<int, int> modifiedTile; //Where the next action is performed

        std::queue<candidateState> bfsQueue;
        candidateState initState(currBoard);
        bfsQueue.push(initState);
        bool goalStateMet = currBoard.isWinState();
        while(goalStateMet == false) {
            candidateState curr = bfsQueue.front(); bfsQueue.pop();
            if(curr.board.isWinState()) {
                goalStateMet = true;
                nextBoard = curr.ancestralState;
                modifiedTile = curr.ancestralModification;
                cost = curr.ancestralState.cost;
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

        board = nextBoard.tileStates;
        return modifiedTile;
    }
};

/**
 * Assumes that the ghost starts on the upper-left and the human is at the lower-right 
 */
class greedyAI: public AI {
    long long int cost = 0;

public:
    greedyAI(std::vector<std::vector<int>>& board): AI(board) {
        this->board = board;
    }

    std::pair<int, int> nextIteration() {
        bool goalStateMet = false;
        boardState currBoard(board, cost); //Insert initial board 
        boardState nextBoard = currBoard.copy(); //The next state of the board
        std::pair<int, int> modifiedTile; //Where the next action is performed
        if(currBoard.isWinState() == false) {
            nextBoard = currBoard.copy();
            std::pair<int, int> ghostPosition = currBoard.getGhostPosition();
            std::pair<int, int> downTile = std::make_pair(ghostPosition.first + 1, ghostPosition.second);

            if(currBoard.canMoveTo(ghostPosition.first, ghostPosition.second + 1)) {
                nextBoard = nextBoard.moveRight();
                modifiedTile = std::make_pair(ghostPosition.first, ghostPosition.second + 1);
                cost = nextBoard.cost;
            } else if(currBoard.canMoveTo(ghostPosition.first + 1, ghostPosition.second)) {
                nextBoard = nextBoard.moveDown();
                modifiedTile = std::make_pair(ghostPosition.first + 1, ghostPosition.second);
                cost = nextBoard.cost;
            } else if(currBoard.canTurnOff(ghostPosition.first, ghostPosition.second + 1)) {
                nextBoard = nextBoard.turnOffRight();
                modifiedTile = std::make_pair(ghostPosition.first, ghostPosition.second + 1);
                cost = nextBoard.cost;
            } else if(currBoard.canTurnOff(ghostPosition.first + 1, ghostPosition.second)) {
                nextBoard = nextBoard.turnOffDown();
                modifiedTile = std::make_pair(ghostPosition.first + 1, ghostPosition.second);
                cost = nextBoard.cost;
            } else {
                assert(currBoard.isWinState());
            }
        }

        board = nextBoard.tileStates;
        return modifiedTile;
    }
};
