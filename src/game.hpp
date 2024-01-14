#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "SDL_events.h"
#include "application.hpp"
#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>

class game : public application_applet {
public:
  game();
  virtual void init() override;
  virtual void update(float pDelta) override;
  virtual void dispose() override;
  virtual void handle_event(SDL_Event &pEvent) override;

private:
  entt::registry mRegistry;
};

#endif // __GAME_HPP__
