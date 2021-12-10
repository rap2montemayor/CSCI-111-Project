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
    enum ModeID { None, MovingChild, MovingGhost, EditingGrid };
    enum ButtonID { NextIter, ShowSolution, AutoPlay, Randomize,
        MoveChild, MoveGhost, EditGrid, Other };
    struct MenuItem {
        sf::RectangleShape rect;
        void (PlayScene::*effect)();
        bool enable;
        ButtonID id;
    };
    sf::Clock clock;
    bool solved, autoPlay;
    std::pair<int, int> ghost, child;
    ModeID mode;
    int rows, cols;
    std::vector<std::vector<int>> board;
    std::vector<std::vector<int>> origBoard;
    std::vector<std::vector<sf::RectangleShape>> gridContents;
public:
    std::vector<sf::Texture> textures;
    std::list<MenuItem> menuItems;
    std::unique_ptr<AI> searchMethod;
    std::vector<std::pair<int, int>> solution;
    PlayScene();
    void processInput(sf::Event& event);
    Transition update();
    void render(sf::RenderWindow& window);
    void generateGrid(int rows, int cols);
    void recolorTile(int r, int c);
    void handleClick(int x, int y);

    // UI functions
    void randomize();
    void reset();
    void nextIteration();
    void showSolution();
    void play();
    void incRows();
    void decRows();
    void incCols();
    void decCols();
    void moveChild();
    void moveGhost();
    void editGrid();
    void hardReset();
};