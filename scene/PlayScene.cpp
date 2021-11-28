#include "Scene.hpp"
#include <random>
#include <chrono>
#include <iostream>
#include <stack>
// Not yet finalized

PlayScene::PlayScene() {
    rows = 16, cols = 16;
    
    // Initialize UI
    mode = ModeID::None;
    
    // next iteration
    sf::RectangleShape nextButton(sf::Vector2f(100, 50));
    nextButton.setPosition(400, 100);
    nextButton.setFillColor(sf::Color::Yellow);
    menuItems.push_back({nextButton, nextIteration, true, ButtonID::NextIter});

    // show solution
    sf::RectangleShape solutionButton(sf::Vector2f(100, 50));
    solutionButton.setPosition(400, 200);
    solutionButton.setFillColor(sf::Color::Blue);
    menuItems.push_back({solutionButton, showSolution, false, ButtonID::ShowSolution});

    // play button
    sf::RectangleShape playButton(sf::Vector2f(100, 50));
    playButton.setPosition(400, 300);
    playButton.setFillColor(sf::Color::Red);
    menuItems.push_back({playButton, play, true, ButtonID::AutoPlay});

    // randomize
    sf::RectangleShape randomButton(sf::Vector2f(100, 50));
    randomButton.setPosition(400, 400);
    randomButton.setFillColor(sf::Color::Green);
    menuItems.push_back({randomButton, randomize, true, ButtonID::Randomize});

    // Move Ghost
    sf::RectangleShape moveGhostButton(sf::Vector2f(100, 50));
    moveGhostButton.setPosition(550, 100);
    moveGhostButton.setFillColor(sf::Color::White);
    menuItems.push_back({moveGhostButton, moveGhost, true, ButtonID::MoveGhost});

    // Move child
    sf::RectangleShape moveChildButton(sf::Vector2f(100, 50));
    moveChildButton.setPosition(550, 200);
    moveChildButton.setFillColor(sf::Color::Cyan);
    menuItems.push_back({moveChildButton, moveChild, true, ButtonID::MoveChild});

    // Edit Grid
    sf::RectangleShape editGridButton(sf::Vector2f(100, 50));
    editGridButton.setPosition(550, 300);
    editGridButton.setFillColor(sf::Color::Magenta);
    menuItems.push_back({editGridButton, editGrid, true, ButtonID::EditGrid});

    // randomize grid
    randomize();
    searchMethod = std::make_unique<DFS>(board);
}

void PlayScene::recolorTile(int r, int c) {
    int state = board[r][c];

    gridContents[r][c].setFillColor(sf::Color::Black);
    gridContents[r][c].setOutlineThickness(0);

    if (state & AI::TileState::Passable) {
        gridContents[r][c].setFillColor(sf::Color::White);
    } else {
        return;
    }

    if (state & AI::TileState::Dark) {
        gridContents[r][c].setFillColor(sf::Color(128, 128, 128));
    }
    
    // tint light blue
    if (state & AI::TileState::ToVisit) {
        gridContents[r][c].setFillColor(sf::Color(64, 64, 128));
    }

    // tint dark blue
    if (state & AI::TileState::Visited) {
        gridContents[r][c].setFillColor(sf::Color(32, 32, 64));
    }

    // outline blue
    if (state & AI::TileState::HasGhost) {
        gridContents[r][c].setOutlineThickness(4);
        gridContents[r][c].setOutlineColor(sf::Color::Blue);
    }

    // outline yellow
    if (state & AI::TileState::HasChild) {
        gridContents[r][c].setOutlineThickness(4);
        gridContents[r][c].setOutlineColor(sf::Color::Yellow);
    }
}

void PlayScene::handleClick(int x, int y) {
    // check if a grid square was clicked
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            sf::RectangleShape &rect = gridContents[i][j];
            sf::Vector2f upperleft = rect.getPosition();
            sf::Vector2f lowerright = upperleft + rect.getSize();
            if (upperleft.x <= x and x <= lowerright.x and upperleft.y <= y and y <= lowerright.y) {
                switch (mode) {
                case ModeID::MovingChild:
                    if (board[i][j] & AI::TileState::HasGhost) {
                        break;
                    } else if (board[i][j] & AI::TileState::HasChild) {
                        break;
                    } else if (not (board[i][j] & AI::TileState::Passable)) {
                        break;
                    } else {
                        board[i][j] |= AI::TileState::HasChild;
                        board[child.first][child.second] &= ~AI::TileState::HasChild;
                        recolorTile(child.first, child.second);
                        recolorTile(i, j);
                        child = {i, j};
                    }
                    break;
                case ModeID::MovingGhost:
                    if (board[i][j] & AI::TileState::HasGhost) {
                        break;
                    } else if (board[i][j] & AI::TileState::HasChild) {
                        break;
                    } else if (not (board[i][j] & AI::TileState::Passable)) {
                        break;
                    } else {
                        board[i][j] |= AI::TileState::HasGhost;
                        board[ghost.first][ghost.second] &= ~AI::TileState::HasGhost;
                        recolorTile(ghost.first, ghost.second);
                        recolorTile(i, j);
                        ghost = {i, j};
                    }
                    break;
                case ModeID::EditingGrid:
                    if (board[i][j] & AI::TileState::HasChild) {
                        break;
                    } else if (board[i][j] & AI::TileState::HasGhost) {
                        break;
                    } else {
                        if (not (board[i][j] & AI::TileState::Passable)) {
                            board[i][j] |= AI::TileState::Passable;
                            board[i][j] |= AI::TileState::Dark;
                        } else if (board[i][j] & AI::TileState::Dark) {
                            board[i][j] &= ~AI::TileState::Dark;
                        } else {
                            board[i][j] &= ~AI::TileState::Passable;
                        }
                        recolorTile(i, j);
                    }
                    break;  
                case ModeID::None:
                    break;
                }
            }
        }
    }

    // check if menu item was clicked
    for (MenuItem& i: menuItems) {
        if (not i.enable) continue;
        sf::Vector2f upperleft = i.rect.getPosition();
        sf::Vector2f lowerright = upperleft + i.rect.getSize();

        if (upperleft.x <= x and x <= lowerright.x and upperleft.y <= y and y <= lowerright.y) {
            switch (mode) {
            case ModeID::MovingChild:
                if (i.id == ButtonID::MoveChild) {
                    (this->*(i.effect))();
                }
                break;
            case ModeID::MovingGhost:
                if (i.id == ButtonID::MoveGhost) {
                    (this->*(i.effect))();
                }
                break;
            case ModeID::EditingGrid:
                if (i.id == ButtonID::EditGrid) {
                    (this->*(i.effect))();
                }
                break;  
            case ModeID::None:
                (this->*(i.effect))();
                break;
            }
        }
    }

}

void PlayScene::processInput(sf::Event& event) {
    // check if input is left click
    // menuSelection = handleinput

    // check if input is left click
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            handleClick(event.mouseButton.x, event.mouseButton.y);
        }
    }
}

Scene::Transition PlayScene::update() {
    if (autoPlay) {
        if (clock.getElapsedTime().asMilliseconds() < 50) {
            return Transition::None;
        }
        clock.restart();
        if (searchMethod->isGoalState()) {
            autoPlay = false;
        }
        nextIteration();
    }
    return Transition::None;
}

void PlayScene::render(sf::RenderWindow& window) {
    // Render grid
    window.clear();
    for (int i = 0; i < rows; ++i) {
        for (sf::RectangleShape& j: gridContents[i]) {
            window.draw(j);
        }
    }

    // render menuitems
    for (MenuItem& i: menuItems) {
        if (not i.enable) continue;
        switch (mode) {
            case ModeID::MovingChild:
                if (i.id == ButtonID::MoveChild) {
                    window.draw(i.rect);
                }
                break;
            case ModeID::MovingGhost:
                if (i.id == ButtonID::MoveGhost) {
                    window.draw(i.rect);
                }
                break;
            case ModeID::EditingGrid:
                if (i.id == ButtonID::EditGrid) {
                    window.draw(i.rect);
                }
                break;  
            case ModeID::None:
                window.draw(i.rect);
                break;
        }
    }
    window.display();
}


// refactor this
void PlayScene::generateGrid(int rows, int cols) {
    std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> randrow(0, rows-1), randcol(0, cols-1);
    std::uniform_real_distribution<double> rand(0.0, 1.0);
    
    // Initialize grid
    board.resize(rows, std::vector<int>(cols));
    for (std::vector<int> &v: board) {
        fill(v.begin(), v.end(), AI::TileState::Passable);
    }

    // Place ghost and child randomly
    int ghostr = randrow(rng), ghostc = randcol(rng), childr, childc;
    do {
        childr = randrow(rng);
        childc = randcol(rng);
    } while (ghostr == childr and ghostc == childc);
    board[ghostr][ghostc] |= AI::TileState::HasGhost | AI::TileState::Dark;
    board[childr][childc] |= AI::TileState::HasChild;
    ghost = {ghostr, ghostc};
    child = {childr, childc};

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

void PlayScene::nextIteration() {
    std::vector<std::pair<int, int>> changes;
    if (not solved) {
        if (searchMethod->isGoalState()) {
            std::cout << "goal found!" << std::endl;
            // look for show solution button
            for (MenuItem &i: menuItems) {
                if (i.id == ButtonID::ShowSolution) {
                    i.enable = true;
                }
            }
            return;
        }
        changes = searchMethod->nextIteration();
    } else {
        changes = searchMethod->nextSolutionStep();    
    }

    for (std::pair<int, int> &p: changes) {
        recolorTile(p.first, p.second);
    }
}

void PlayScene::showSolution() {
    searchMethod->constructSolution();
    reset();
    solved = true;
}

void PlayScene::reset() {
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            board[i][j] = origBoard[i][j];
            recolorTile(i, j);
        }
    }
}

void PlayScene::play() {
    autoPlay = not autoPlay;
}

void PlayScene::randomize() {
    int sizei = 16, sizej = 16;
    solved = false;
    autoPlay = false;
    generateGrid(rows, cols);
    origBoard.resize(rows, std::vector<int>(cols));

    // populate grid
    gridContents.resize(rows);
    for (int i = 0; i < rows; ++i) {
        gridContents[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            origBoard[i][j] = board[i][j];
            gridContents[i][j] = sf::RectangleShape(sf::Vector2f(sizei, sizej));
            gridContents[i][j].setPosition(16+sizei*i + 8*i, 16+sizej*j + 8*j);
            recolorTile(i, j);
        }
    }

    for (MenuItem& i: menuItems) {
        if (i.id == ButtonID::ShowSolution) {
            i.enable = false;
        }
    }

    if (searchMethod) {
        searchMethod->reset();
    }
}

void PlayScene::hardReset() {
    reset();
    searchMethod->reset();
    autoPlay = false;
    solved = false;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            recolorTile(i, j);
        }
    }
}

void PlayScene::moveChild() {
    if (mode == ModeID::None) {
        hardReset();
        mode = ModeID::MovingChild;
    } else {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                origBoard[i][j] = board[i][j];
            }
        }
        mode = ModeID::None;
    }
}

void PlayScene::moveGhost() {
    if (mode == ModeID::None) {
        hardReset();
        mode = ModeID::MovingGhost;
    } else {
        board[ghost.first][ghost.second] |= AI::TileState::Dark;
        recolorTile(ghost.first, ghost.second);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                origBoard[i][j] = board[i][j];
            }
        }
        mode = ModeID::None;
    }
}

void PlayScene::editGrid() {
    if (mode == ModeID::None) {
        hardReset();
        mode = ModeID::EditingGrid;
    } else {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                origBoard[i][j] = board[i][j];
            }
        }
        mode = ModeID::None;
    }
}
