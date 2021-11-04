#include "Scene.hpp"
#include "../base/Game.hpp"
#include <iostream>

// Not yet finalized

MenuScene::MenuScene() {
    // populate menuitems
    menuSelection = None;
    
    // play game
    sf::RectangleShape playButton(sf::Vector2f(400, 100));
    playButton.setPosition(200.0, 250.0);
    playButton.setFillColor(sf::Color::Yellow);
    menuItems.push_back({playButton, Play});

    // exit
    sf::RectangleShape exitButton(sf::Vector2f(400, 100));
    exitButton.setPosition(200.0, 400.0);
    exitButton.setFillColor(sf::Color::Yellow);
    menuItems.push_back({exitButton, Exit});
}

void MenuScene::handleClick(int x, int y) {
    // Check if a button was clicked on
    for (MenuItem& i: menuItems) {
        sf::Vector2f upperleft = i.rect.getPosition();
        sf::Vector2f lowerright = upperleft + i.rect.getSize();

        if (upperleft.x <= x and x <= lowerright.x
                and upperleft.y <= y and y <= lowerright.y) {
            menuSelection = i.option;
        }
    }
}

void MenuScene::processInput(sf::Event& event) {
    // check if input is left click
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            handleClick(event.mouseButton.x, event.mouseButton.y);
        }
    }
}

Scene::Transition MenuScene::update() {
    if (menuSelection != None) {
        return menuSelection;
    }
    return None;
}

void MenuScene::render(sf::RenderWindow& window) {
    // render menuitems
    window.clear();
    for (MenuItem& i: menuItems) {
        window.draw(i.rect);
    }
    window.display();
}
