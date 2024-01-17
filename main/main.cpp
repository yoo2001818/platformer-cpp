#include "application.hpp"
#include "game.hpp"
#include <memory>

int main() {
  auto gameVar = std::make_unique<platformer::game>();
  platformer::application app(std::move(gameVar));
  app.start();
  return 0;
}
