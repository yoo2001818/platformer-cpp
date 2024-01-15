#include "game.hpp"

using namespace platformer;

game::game() : mRegistry() {}

void game::init() { auto entity = this->mRegistry.create(); }

void game::update(float pDelta) {}

void game::dispose() {}

void game::handle_event(SDL_Event &pEvent) {}
