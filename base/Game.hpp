#pragma once
#include <SFML/Graphics.hpp>
#include "../scene/Scene.hpp"
#include <memory>

class Game {
    enum GameState { Uninitialized, Menu, Playing, Exiting };
    static GameState gameState;
    static std::unique_ptr<Scene> gameScene;
    static sf::RenderWindow mainWindow;
    static sf::Event event;
    static void handleTransition(GameState& previousState);
    static void updateGameState(Scene::Transition sceneTransition);
    static void processInput();

public:
    static void start();
};