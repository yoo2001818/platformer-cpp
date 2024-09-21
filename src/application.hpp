#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include "SDL_events.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <memory>

namespace platformer {
class application;

class application_applet {
public:
  virtual void init(application &pApplication) = 0;
  virtual void update(application &pApplication, float pDelta) = 0;
  virtual void dispose() = 0;
  /**
   * @return int 0 means netural - should be sent to imgui and the game. <0
   * means it should be ignored by the game, >0 means it must be process by the
   * game.
   */
  virtual int peek_event(application &pApplication, SDL_Event &pEvent) = 0;
  virtual void handle_event(application &pApplication, SDL_Event &pEvent) = 0;
};

class application {
public:
  application(std::unique_ptr<application_applet> &&pApplet);
  void start();
  SDL_Window *&window();

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
