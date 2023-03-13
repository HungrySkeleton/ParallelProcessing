#include "Game.hpp"

Game::Game(int w, int h) : width(w), height(h) {}
int Game::area()
{
    return width*height;
}