#include "Scene.hpp"

PlayScene::PlayScene() {
    cols = 2;
    rows = 2;
    step = 0;
    autoPlay = false;
    extensions = false;
    solved = false;
    mode = ModeID::None;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    child = {1, 1};
    ghost = {0, 0};

    initializeGrid();
    board[0][0] |= tileState::HAS_GHOST;
    board[1][0] |= tileState::HAS_LIGHT;
    board[1][1] |= tileState::HAS_PERSON | tileState::HAS_LIGHT;
    populate();
    useBFS();

    loadTextures();
    initializeMenu();
}

// public

void PlayScene::processInput(sf::Event& event) {
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
        if (searchMethod->isWinState() and not solved) {
            autoPlay = false;
        } else if (solved and step >= solution.size()-1) {
            autoPlay = false;
        }
        nextIteration();
    }
    return Transition::None;
}

void PlayScene::render(sf::RenderWindow& window) {
    // Render grid
    double tilesize = 450 / std::max(rows, cols);
    window.clear();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            window.draw(gridContents[i][j]);
            if (board[i][j] & tileState::HAS_GHOST) {
                // draw ghost
                sf::RectangleShape rect(sf::Vector2f(tilesize, tilesize));
                rect.setPosition(50 + tilesize*j, 75 + tilesize*i);
                rect.setTexture(&textures[1]);
                window.draw(rect);
            }
            if (board[i][j] & tileState::HAS_PERSON) {
                // draw child
                sf::RectangleShape rect(sf::Vector2f(tilesize, tilesize));
                rect.setPosition(50 + tilesize*j, 75 + tilesize*i);
                rect.setTexture(&textures[0]);
                window.draw(rect);
            }
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

// private

double PlayScene::rand() {
    return std::uniform_real_distribution<double>(0.0, 1.0)(rng);
}

int PlayScene::randcol() {
    return std::uniform_int_distribution<int>(0, cols-1)(rng);
}

int PlayScene::randrow() {
    return std::uniform_int_distribution<int>(0, rows-1)(rng);
}

void PlayScene::decCols() {
    if (cols <= 2) return;
    --cols;
    randomize();
}

void PlayScene::decRows() {
    if (rows <= 2) return;
    --rows;
    randomize();
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
        hardReset();
    }
}

void PlayScene::generateGrid() {
    initializeGrid();
    placeGhostAndChild();
    randomizeBoard();
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
                    if (board[i][j] & tileState::HAS_GHOST) {
                        break;
                    } else if (board[i][j] & tileState::HAS_PERSON) {
                        break;
                    } else {
                        board[i][j] |= tileState::HAS_PERSON;
                        board[child.first][child.second] &= ~tileState::HAS_PERSON;
                        recolorTile(child.first, child.second);
                        recolorTile(i, j);
                        child = {i, j};
                    }
                    break;
                case ModeID::MovingGhost:
                    if (board[i][j] & tileState::HAS_GHOST) {
                        break;
                    } else if (board[i][j] & tileState::HAS_PERSON) {
                        break;
                    } else {
                        board[i][j] |= tileState::HAS_GHOST;
                        board[ghost.first][ghost.second] &= ~tileState::HAS_GHOST;
                        recolorTile(ghost.first, ghost.second);
                        recolorTile(i, j);
                        ghost = {i, j};
                    }
                    break;
                case ModeID::EditingGrid:
                    if (not (board[i][j] & tileState::HAS_GHOST)) {
                        board[i][j] ^= tileState::HAS_LIGHT;
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

void PlayScene::hardReset() {
    reset();
    searchMethod->reset();
    autoPlay = false;
    solved = false;
    for (MenuItem& i: menuItems) {
        if (i.id == ButtonID::ShowSolution) {
            i.enable = false;
        }
    }
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            recolorTile(i, j);
        }
    }
}

void PlayScene::incCols() {
    if (cols >= 16) return;
    ++cols;
    randomize();
}

void PlayScene::incRows() {
    if (rows >= 16) return;
    ++rows;
    randomize();
}

void PlayScene::initializeGrid() {
    board.resize(rows);
    for (std::vector<int>& v: board) {
        v.resize(cols);
        fill(v.begin(), v.end(), 0);
    }
}

void PlayScene::initializeMenu() {
    struct Entry {
        int sizex, sizey, posx, posy;
        sf::Color color;
        void (PlayScene::*effect)();
        bool enable;
        ButtonID id;
        int textureid;
    };
    Entry entries[] = {
        // Next iteration
        {100, 50, 525, 50, sf::Color::White, nextIteration, true, ButtonID::Default, 10},
        // Show solution
        {100, 50, 525, 125, sf::Color::White, showSolution, false, ButtonID::ShowSolution, 13},
        // Play button
        {100, 50, 525, 200, sf::Color::White, play, true, ButtonID::Default, 6},
        // Randomize
        {100, 50, 525, 275, sf::Color::White, randomize, false, ButtonID::Extra, 11},

        // BFS
        {100, 50, 525, 425, sf::Color::White, useBFS, true, ButtonID::Default, 15},
        // Greedy
        {100, 50, 525, 500, sf::Color::White, useGreedy, true, ButtonID::Default, 16},

        // Move ghost
        {100, 50, 650, 50, sf::Color::White, moveGhost, true, ButtonID::MoveGhost, 9},
        // Move child
        {100, 50, 650, 125, sf::Color::White, moveChild, true, ButtonID::MoveChild, 8},
        // Edit Grid
        {100, 50, 650, 200, sf::Color::White, editGrid, true, ButtonID::EditGrid, 7},
        // Increment rows
        {100, 50, 650, 275, sf::Color::White, incRows, false, ButtonID::Extra, 3},
        // Decrement rows
        {100, 50, 650, 350, sf::Color::White, decRows, false, ButtonID::Extra, 5},
        // Increment columns
        {100, 50, 650, 425, sf::Color::White, incCols, false, ButtonID::Extra, 2},
        // Decrement columns
        {100, 50, 650, 500, sf::Color::White, decCols, false, ButtonID::Extra, 4},
        // Toggle Extensions
        {100, 50, 25, 525, sf::Color::White, toggleExtensions, true, ButtonID::ExtraToggle, 14},
    };

    sf::RectangleShape button;
    for (Entry &i: entries) {
        button.setSize(sf::Vector2f(i.sizex, i.sizey));
        button.setPosition(i.posx, i.posy);
        button.setFillColor(i.color);
        button.setTexture(&textures[i.textureid]);
        menuItems.push_back({button, i.effect, i.enable, i.id});
    }
}

void PlayScene::loadTextures() {
        std::string texturePaths[] = {
        "assets/child.png",
        "assets/ghost.png",
        
        "assets/+cols.png",
        "assets/+rows.png",
        "assets/-cols.png",
        "assets/-rows.png",
        "assets/auto.png",
        "assets/edittiles.png",
        "assets/movechild.png",
        "assets/moveghost.png",
        "assets/next.png",
        "assets/randomize.png",
        "assets/reset.png",
        "assets/solution.png",
        "assets/toggle.png",

        "assets/bfs.png",
        "assets/greedy.png",
    };
    for (std::string &i: texturePaths) {
        textures.emplace_back();
        sf::Texture& cur = textures.back();
        if (not cur.loadFromFile(i)) {
            std::cout << "Error loading texture " << i << std::endl;
        }
    }
    textures.emplace_back();
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
        hardReset();
    }
}

void PlayScene::moveGhost() {
    if (mode == ModeID::None) {
        hardReset();
        mode = ModeID::MovingGhost;
    } else {
        board[ghost.first][ghost.second] &= ~tileState::HAS_LIGHT;
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                origBoard[i][j] = board[i][j];
            }
        }
        mode = ModeID::None;
        hardReset();
    }
}

void PlayScene::nextIteration() {
    std::vector<std::pair<int, int>> changes;
    if (not solved) {
        if (searchMethod->isWinState()) {
            // enable show solution button
            std::cout << "Solution has been found" << std::endl;
            solved = true;
            for (MenuItem &i: menuItems) {
                if (i.id == ButtonID::ShowSolution) {
                    i.enable = true;
                }
            }
            return;
        } else {
            std::cout << "Showing next state to check" << std::endl;
        }
        searchMethod->nextIteration();
    } else {
        nextSolutionStep();
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            recolorTile(i, j);
        }
    }
}

void PlayScene::nextSolutionStep() {
    if (step+1 >= solution.size()) {
        std::cout << "Done showing solution" << std::endl;
        return;
    }
    std::cout << "Showing next solution step" << std::endl;
    ++step;
    std::vector<std::vector<int>>& nextState = solution[step];
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            board[i][j] = nextState[i][j];
        }
    }
}

void PlayScene::placeGhostAndChild() {
    int ghostr = randrow(), ghostc = randcol(), childr, childc;
    do {
        childr = randrow();
        childc = randcol();
    } while (ghostr == childr and ghostc == childc);
    board[ghostr][ghostc] |= tileState::HAS_GHOST;
    board[childr][childc] |= tileState::HAS_PERSON | tileState::HAS_LIGHT;
    ghost = {ghostr, ghostc};
    child = {childr, childc};
}

void PlayScene::play() {
    autoPlay = not autoPlay;
}

void PlayScene::populate() {
    double tilesize = 450.0 / std::max(rows, cols);
    origBoard.resize(rows);
    gridContents.resize(rows);
    for (int i = 0; i < rows; ++i) {
        origBoard[i].resize(cols);
        gridContents[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            origBoard[i][j] = board[i][j];
            gridContents[i][j].setSize(sf::Vector2f(tilesize, tilesize));
            gridContents[i][j].setPosition(50 + tilesize*j, 75 + tilesize*i);
            recolorTile(i, j);
        }
    }
}

void PlayScene::randomize() {
    solved = false;
    autoPlay = false;

    generateGrid();
    populate();

    for (MenuItem& i: menuItems) {
        if (i.id == ButtonID::ShowSolution) {
            i.enable = false;
        }
    }

    if (searchMethod) {
        searchMethod->reset();
    }
}

void PlayScene::randomizeBoard() {
    for (std::vector<int>& v : board) {
        for (int& i: v) {
            // make light
            if (rand() < 1.0/3.0) {
                i |= tileState::HAS_LIGHT;
            }
        }
    }
}

void PlayScene::recolorTile(int r, int c) {
    int state = board[r][c];

    gridContents[r][c].setFillColor(sf::Color::White);
    gridContents[r][c].setOutlineThickness(-std::ceil(450 / std::max(rows, cols) * 0.1));
    gridContents[r][c].setOutlineColor(sf::Color::Black);

    // color dark
    if (not (state & tileState::HAS_LIGHT)) {
        gridContents[r][c].setFillColor(sf::Color(128, 128, 128));
    }

    // outline blue
    if (state & tileState::HAS_GHOST) {
        gridContents[r][c].setOutlineColor(sf::Color::Blue);
    }

    // outline yellow
    if (state & tileState::HAS_PERSON) {
        gridContents[r][c].setOutlineColor(sf::Color::Yellow);
    }
}

void PlayScene::reset() {
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            board[i][j] = origBoard[i][j];
            recolorTile(i, j);
        }
    }
}

void PlayScene::showSolution() {
    solution = searchMethod->getHistory();
    reset();
    solved = true;
    step = 0;
    std::cout << "Showing solution steps\n";
    std::cout << "Solution has " << solution.size()-1 << " steps" << std::endl;
}

void PlayScene::toggleExtensions() {
    extensions = not extensions;
    for (MenuItem& i: menuItems) {
        if (i.id == ButtonID::Extra) {
            i.enable = not i.enable;
        }
    }
}

void PlayScene::useBFS() {
    std::cout << "using BFS" << std::endl;
    searchMethod = std::make_unique<bfsAI>(board);
    hardReset();
}

void PlayScene::useGreedy() {
    std::cout << "using greedy" << std::endl;
    searchMethod = std::make_unique<greedyAI>(board);
    hardReset();
}