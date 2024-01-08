#include "application.hpp"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <ctime>

application::application(std::unique_ptr<application_applet> &&pApplet)
    : mApplet(std::move(pApplet)) {}

void application::start() {
  if (this->init() != 0)
    return;
  while (true) {
    SDL_Event event;
    bool exit = false;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        exit = true;
        break;
      }
      // TODO: Handle additional events
    }
    if (exit)
      break;

    uint64_t beginTime = SDL_GetTicks64();
    this->update();
    uint64_t endTime = SDL_GetTicks64();
    int32_t deltaTime = static_cast<int32_t>(endTime - beginTime);
    int32_t sleepTime = 12 - deltaTime;

    if (sleepTime > 0) {
      SDL_Delay(sleepTime);
    }
  }
  this->dispose();
}

int application::init() {
  std::srand(std::time(nullptr));

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
  this->mWindow = SDL_CreateWindow("PlatformerCpp", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, 1024, 768,
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  this->mGLContext = SDL_GL_CreateContext(this->mWindow);
  SDL_GL_MakeCurrent(this->mWindow, this->mGLContext);
  if (glewInit() != GLEW_OK) {
    return 1;
  }

  this->mApplet->init();
  return 0;
}

void application::update() {
  this->mApplet->update();
  SDL_GL_SwapWindow(this->mWindow);
}

void application::dispose() {
  this->mApplet->dispose();
  SDL_GL_DeleteContext(this->mGLContext);
  SDL_DestroyWindow(this->mWindow);
  SDL_Quit();
}
