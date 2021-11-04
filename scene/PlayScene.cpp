#include "Scene.hpp"

// Not yet finalized

PlayScene::PlayScene() {
    rows = 10, cols = 10;
    int sizei = 16, sizej = 16;
    // populate background
    // populate foreground

    // populate grid
    board.resize(rows);
    gridContents.resize(rows);
    for (int i = 0; i < rows; ++i) {
        board[i].resize(cols);
        gridContents[i].resize(cols);
        for (int j = 0; j < cols; ++j) {
            gridContents[i][j].reset(new sf::RectangleShape(sf::Vector2f(sizei, sizej)));
            gridContents[i][j]->setPosition(sizei*i + 8*i, sizej*j + 8*j);
        }
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
    for (int i = 0; i < 10; ++i) {
        for (std::unique_ptr<sf::Shape> const &j : gridContents[i]) {
            window.draw(*j);
        }
    }
    // for (std::unique_ptr<sf::Drawable> const &i: background) {
        // window.draw(*i);
    // }
    window.display();
}
