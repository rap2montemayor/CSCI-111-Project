#pragma once
#include <SFML/Graphics.hpp>
#include <list>
#include <memory>
#include "../ai/AI.hpp"

// Not yet finalized

class Scene {
public:
    enum Transition { None, Pause, Menu, Play, Exit };
    virtual void processInput(sf::Event& event) = 0;
    virtual Transition update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
};


class MenuScene : public Scene {
    struct MenuItem {
        sf::RectangleShape rect;
        Transition option;
    };
    std::list<MenuItem> menuItems;
    Transition menuSelection;

    void handleClick(int x, int y);

public:
    MenuScene();
    void processInput(sf::Event& event);
    Transition update();
    void render(sf::RenderWindow& window);
};


class PlayScene : public Scene {
    int rows, cols;
    std::vector<std::vector<int>> board; 
    std::vector<std::vector<std::unique_ptr<sf::Shape>>> gridContents;
public:
    PlayScene();
    void processInput(sf::Event& event);
    Transition update();
    void render(sf::RenderWindow& window);
    void generateGrid(int rows, int cols);
    void recolorTile(int r, int c);
};