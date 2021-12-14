#include "Scene.hpp"
#include "../base/Game.hpp"
#include <iostream>

// Not yet finalized

MenuScene::MenuScene() {
    // populate menuitems
    menuSelection = None;

    std::string texturePaths[] = {
        "assets/start.png",
        "assets/exit.png",
        "assets/title.png"
    };

    for (std::string &i: texturePaths) {
        textures.emplace_back();
        sf::Texture& cur = textures.back();
        if (not cur.loadFromFile(i)) {
            std::cout << "Error loading texture " << i << std::endl;
        }
    }

    struct Entry {
        int sizex, sizey, posx, posy;
        sf::Color color;
        Transition transition;
        int textureid;
    };

    Entry entries[] = {
        // Start
        {400, 100, 200, 250, sf::Color::White, Transition::Play, 0},
        // Exit
        {400, 100, 200, 400, sf::Color::White, Transition::Exit, 1},
    };

    sf::RectangleShape button;
    for (Entry& i: entries) {
        button.setSize(sf::Vector2f(i.sizex, i.sizey));
        button.setPosition(i.posx, i.posy);
        button.setFillColor(i.color);
        button.setTexture(&textures[i.textureid]);
        menuItems.push_back({button, i.transition});
    }
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
    sf::RectangleShape bg(sf::Vector2f(800, 600));
    bg.setTexture(&textures[2]);
    window.draw(bg);
    for (MenuItem& i: menuItems) {
        window.draw(i.rect);
    }
    window.display();
}
