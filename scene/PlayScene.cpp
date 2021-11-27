#include "Scene.hpp"
#include <random>
#include <chrono>
#include <iostream>
#include <stack>
// Not yet finalized

PlayScene::PlayScene() {
    rows = 10, cols = 10;
    int sizei = 16, sizej = 16;
    // populate background
    // populate foreground

    // generate grid
    generateGrid(rows, cols);

    // populate grid
    gridContents.resize(rows);
    for (int i = 0; i < rows; ++i) {
        gridContents[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            gridContents[i][j] = std::make_unique<sf::RectangleShape>(sf::Vector2f(sizei, sizej));
            gridContents[i][j]->setPosition(sizei*i + 8*i, sizej*j + 8*j);
            recolorTile(i, j);
        }
    }
}

void PlayScene::recolorTile(int r, int c) {
    int state = board[r][c];

    gridContents[r][c]->setFillColor(sf::Color::Black);
    gridContents[r][c]->setOutlineThickness(0);

    if (state & AI::TileState::Passable) {
        gridContents[r][c]->setFillColor(sf::Color::White);
    } else {
        return;
    }

    if (state & AI::TileState::Dark) {
        gridContents[r][c]->setFillColor(sf::Color(128, 128, 128));
    }
    
    // tint light blue
    if (state & AI::TileState::ToVisit) {
        gridContents[r][c]->setFillColor(sf::Color(64, 64, 128));
    }

    // tint dark blue
    if (state & AI::TileState::Visited) {
        gridContents[r][c]->setFillColor(sf::Color(32, 32, 64));
    }

    // outline blue
    if (state & AI::TileState::HasGhost) {
        gridContents[r][c]->setOutlineThickness(4);
        gridContents[r][c]->setOutlineColor(sf::Color::Blue);
    }

    // outline yellow
    if (state & AI::TileState::HasChild) {
        gridContents[r][c]->setOutlineThickness(4);
        gridContents[r][c]->setOutlineColor(sf::Color::Yellow);
    }
}

void PlayScene::processInput(sf::Event& event) {
    // check if input is left click
    // menuSelection = handleinput
}

Scene::Transition PlayScene::update() {
    // check if paused
    // update gamestate if menuselection != None
    return None;
}

void PlayScene::render(sf::RenderWindow& window) {
    // render menuitems
    window.clear();
    for (int i = 0; i < rows; ++i) {
        for (std::unique_ptr<sf::Shape> const &j : gridContents[i]) {
            window.draw(*j);
        }
    }
    // for (std::unique_ptr<sf::Drawable> const &i: background) {
        // window.draw(*i);
    // }
    window.display();
}


// refactor this
void PlayScene::generateGrid(int rows, int cols) {
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> randrow(0, rows-1), randcol(0, cols-1);
    std::uniform_real_distribution<double> rand(0.0, 1.0);
    
    // Initialize grid
    board.resize(rows, std::vector<int>(cols, AI::TileState::Passable));

    // Place ghost and child randomly
    int ghostr = randrow(rng), ghostc = randcol(rng), childr, childc;
    do {
        childr = randrow(rng);
        childc = randcol(rng);
    } while (ghostr == childr and ghostc == childc);
    board[ghostr][ghostc] |= AI::TileState::HasGhost | AI::TileState::Dark;
    board[childr][childc] |= AI::TileState::HasChild;

    // Randomize board state
    // 2/3 chance of a tile to be dark
    // 1/3 chance of a tile to be impassable
    for (std::vector<int>& v : board) {
        for (int& i: v) {
            // make dark
            if (rand(rng) < 2.0/3.0) {
                i |= AI::TileState::Dark;
            }
            //make impassable
            if (rand(rng) < 1.0/3.0) {
                i &= ~AI::TileState::Passable;
            }
        }
    }

    // Ensure there's a solution
    // DFS from ghost to child. next tiles are chosen randomly
    // make all tiles in that path passable
    std::vector<std::vector<bool>> vis(rows, std::vector<bool>(cols));
    std::stack<std::pair<int, int>> stk({{ghostr, ghostc}});
    std::pair<int, int> d[] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
    while (not stk.empty()) {
        int curr = stk.top().first, curc = stk.top().second;
        vis[curr][curc] = true;

        if (curr == childr and curc == childc) {
            break;
        }

        bool pushed = false;
        std::shuffle(d, d+4, rng);
        for (int i = 0; i < 4; ++i) {
            int nextr = curr+d[i].first, nextc = curc+d[i].second;
            if (nextr < 0 or nextr >= rows or nextc < 0 or nextc >= cols or vis[nextr][nextc]) {
                continue;
            }
            pushed = true;
            stk.push({nextr, nextc});
            break;
        }
        if (not pushed) {
            stk.pop();
        }
    }
    while (not stk.empty()) {
        board[stk.top().first][stk.top().second] |= AI::TileState::Passable;
        stk.pop();
    }
}