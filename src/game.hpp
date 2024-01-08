#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "application.hpp"
#include <entt/entt.hpp>

class game : public application_applet {
public:
  game();
  virtual void init();
  virtual void update();
  virtual void dispose();

private:
};

#endif // __GAME_HPP__
