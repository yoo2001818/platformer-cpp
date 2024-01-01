#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

class application {
public:
  application();
  void start();

private:
  SDL_Window *mWindow;
  SDL_GLContext mGLContext;
  int init();
  void update();
  void dispose();
};

#endif // __APPLICATION_HPP__
