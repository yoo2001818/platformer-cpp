#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "SDL_events.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <memory>

namespace platformer {
class application_applet {
public:
  virtual void init() = 0;
  virtual void update(float pDelta) = 0;
  virtual void dispose() = 0;
  virtual void handle_event(SDL_Event &pEvent) = 0;
};

class application {
public:
  application(std::unique_ptr<application_applet> &&pApplet);
  void start();

private:
  SDL_Window *mWindow;
  SDL_GLContext mGLContext;
  std::unique_ptr<application_applet> mApplet;
  int init();
  void update(float pDelta);
  void dispose();
};
}; // namespace platformer

#endif // __APPLICATION_HPP__
