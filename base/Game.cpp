#include "Game.hpp"
#include <memory>

// Initialization
Game::GameState Game::gameState = Uninitialized;
std::unique_ptr<Scene> Game::gameScene = std::make_unique<MenuScene>();
sf::RenderWindow Game::mainWindow;
sf::Event Game::event;

void Game::start() {
    // Make sure this isn't called more than once
    if (gameState != Uninitialized) {
        return;
    }

    // Initialize window, set game state
    mainWindow.create(sf::VideoMode(800, 600), "nice");

    // Additional variable to handle state transitions
    gameState = Menu;
    GameState previousState = gameState;
    Scene::Transition sceneTransition;

    // Main game loop
    while (gameState != Exiting) {

        // Process input and pass to scene
        processInput();

        // Update scene and possibly game state
        updateGameState(gameScene->update());

        // Render scene
        gameScene->render(mainWindow);

        // Handle state transition
        handleTransition(previousState);
    }

    // Todo: Additional cleanup?
    mainWindow.close();
}

void Game::processInput() {
    while (mainWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            gameState = Exiting;
        }
        gameScene->processInput(event);
    }
}

void Game::updateGameState(Scene::Transition sceneTransition) {
    switch (sceneTransition) {
        case Scene::Transition::Menu:
            gameState = Menu;
            break;
        case Scene::Transition::Play:
            gameState = Playing;
            break;
        case Scene::Transition::Exit:
            gameState = Exiting;
            break;
    }
}

void Game::handleTransition(GameState& previousState) {
    if (previousState == gameState) {
        return;
    }

    switch (gameState) {
        case Playing:
            gameScene = std::make_unique<PlayScene>();
            break;
        case Menu:
            gameScene = std::make_unique<MenuScene>();
            break;
    }
}