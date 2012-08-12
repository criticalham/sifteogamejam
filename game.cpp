/*
 * Explorathon main game class
 */
#include "game.h"
#include <sifteo.h>

void Game::init() {
    reset();

    LOG("init() completed\n");
}

void Game::reset() {
    foundKey = false;
    foundChest = false;

    keyX = 5;
    keyY = 20;
    chestX = 10;
    chestY = 20;
}

void Game::run() {
    System::paint();
}

/**
  * Marks that a given location has been visited
  */
void Game::visited(int x, int y) {
    // TODO: Check for key
    LOG("Checking for key %d, %d, at %d, %d\n", keyX, keyY, x, y);
    // TODO: Check for chest
    LOG("Checking for chest %d, %d, at %d, %d\n", chestX, chestY, x, y);
}