// #include "AI.hpp"

// DFS::DFS(std::vector<std::vector<int>> &board) : board(board) {
//     reset();
// }

// std::vector<std::pair<int, int>> DFS::nextIteration() {
//     std::vector<std::pair<int, int>> changes;

//     if (toVisit.empty()) {
//         std::cout << "nothing in stack" << std::endl;
//         return changes;
//     }
//     // Get current tile
//     std::pair<int, int> current = toVisit.top();

//     // Process current tile
//     changes.push_back(current);
//     int& r = current.first, c = current.second;

//     bool pushed = false;
//     if (board[r][c] & TileState::Dark) {
//         // If dark, move to it
//         board[ghost.first][ghost.second] &= ~TileState::HasGhost;
//         changes.push_back(ghost);
//         ghost = {r, c};
//         board[r][c] |= TileState::HasGhost;
//         board[r][c] |= TileState::Visited;

//         // Push one unvisited child to the stack
//         // up, right, down, left
//         int dr[] = {-1, 0, 1, 0}, dc[] = {0, 1, 0, -1};
//         for (int i = 0; i < 4; ++i) {
//             int nextr = r+dr[i], nextc = c+dc[i];
//             if (not valid(nextr, nextc) or visited(nextr, nextc) or not passable(nextr, nextc)) {
//                 continue;
//             }
//             pushed = true;
//             board[nextr][nextc] |= TileState::ToVisit;
//             toVisit.push({nextr, nextc});
//             changes.push_back({nextr, nextc});
//             break;
//         }
//     } else {
//         // If light, make it dark
//         board[r][c] |= TileState::Dark;
//         toVisit.push({r, c});
//         changes.push_back({r, c});
//         pushed = true;
//     }

//     // Pop if no more updates
//     if (not pushed and not isGoalState()) {
//         toVisit.pop();
//     }

//     return changes;
// }

// std::vector<std::pair<int, int>> DFS::nextSolutionStep() {
//     std::vector<std::pair<int, int>> changes;
//     if (solutionstep >= solution.size()) {
//         std::cout << "solution done" << std::endl;
//         return changes;
//     } 

//     int& r = solution[solutionstep].first, c = solution[solutionstep].second;
//     changes.push_back({r, c});
//     ++solutionstep;
//     if (board[r][c] & TileState::Dark) {
//         board[ghost.first][ghost.second] &= ~TileState::HasGhost;
//         board[ghost.first][ghost.second] |= TileState::Visited;
//         board[r][c] |= TileState::HasGhost;
//         board[r][c] |= TileState::Visited;
//         changes.push_back(ghost);
//         ghost = {r, c};
//     } else {
//         board[r][c] |= TileState::Dark;
//     }

//     return changes;
// }

// void DFS::constructSolution() {
//     if (solution.empty()) {
//         while (not toVisit.empty()) {
//             if (toVisit.top() == ghost) {
//                 solution.push_back(toVisit.top());
//                 toVisit.pop();
//                 break;
//             }
//             toVisit.pop();
//         }
//         while (not toVisit.empty()) {
//             solution.push_back(toVisit.top());
//             toVisit.pop();
//         }
//         std::reverse(solution.begin(), solution.end());
//     }   
//     solutionstep = 0;
//     ghost = solution[0];
// }

// bool DFS::isGoalState() {
//     int& r = ghost.first, c = ghost.second;
//     return board[r][c] & TileState::HasChild;
// }

// void DFS::reset() {
//     solutionstep = 0;
//     while (not toVisit.empty()) {
//         toVisit.pop();
//     }
//     solution.clear();
//     for (int i = 0; i < board.size(); ++i) {
//         for (int j = 0; j < board.size(); ++j) {
//             if (board[i][j] & TileState::HasGhost) {
//                 ghost = {i, j};
//                 toVisit.push({i, j});
//                 break;
//             }
//         }
//     }
// }

// bool DFS::valid(int r, int c) {
//     return r >= 0 and r < board.size()
//         and c >= 0 and c < board[0].size();
// }

// bool DFS::visited(int r, int c) {
//     return board[r][c] & TileState::Visited;
// }

// bool DFS::passable(int r, int c) {
//     return board[r][c] & TileState::Passable;
// }
