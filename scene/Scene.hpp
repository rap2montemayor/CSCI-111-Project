#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <memory>
#include <random>
#include <stack>
#include <string>
#include <vector>
#include "../ai/AI.hpp"

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
    std::vector<sf::Texture> textures;
    Transition menuSelection;

    void handleClick(int x, int y);

public:
    MenuScene();
    void processInput(sf::Event& event);
    Transition update();
    void render(sf::RenderWindow& window);
};


class PlayScene : public Scene {
    // [l,r]
    enum ButtonID { Default, Extra, ExtraToggle, ShowSolution,
        MoveChild, MoveGhost, EditGrid };
    enum ModeID { None, MovingChild, MovingGhost, EditingGrid };

    struct MenuItem {
        sf::RectangleShape rect;
        void (PlayScene::*effect)();
        bool enable;
        ButtonID id;
    };

    bool autoPlay, extensions, solved;
    int cols, rows, step;
    ModeID mode;
    sf::Clock clock;
    std::list<MenuItem> menuItems;
    std::mt19937 rng;
    std::pair<int, int> child, ghost;
    std::unique_ptr<AI> searchMethod;
    std::vector<sf::Texture> textures;
    std::vector<std::vector<std::vector<int>>> solution;
    std::vector<std::vector<int>> board;
    std::vector<std::vector<int>> origBoard;
    std::vector<std::vector<sf::RectangleShape>> gridContents;

    double rand();
    int randcol();
    int randrow();
    void decCols();
    void decRows();
    void editGrid();
    void generateGrid();
    void handleClick(int x, int y);
    void hardReset();
    void incCols();
    void incRows();
    void initializeGrid();
    void initializeMenu();
    void loadTextures();
    void moveChild();
    void moveGhost();
    void nextIteration();
    void nextSolutionStep();
    void placeGhostAndChild();
    void play();
    void populate();
    void randomize();
    void randomizeBoard();
    void recolorTile(int r, int c);
    void reset();
    void showSolution();
    void toggleExtensions();

public:
    PlayScene();
    void processInput(sf::Event& event);
    Transition update();
    void render(sf::RenderWindow& window);
};