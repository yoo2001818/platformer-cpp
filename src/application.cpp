#include "application.hpp"
#include "SDL_timer.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_video.h>
#include <ctime>

using namespace platformer;

application::application(std::unique_ptr<application_applet> &&pApplet)
    : mApplet(std::move(pApplet)) {}

void application::start() {
  if (this->init() != 0)
    return;
  uint64_t prevTime = SDL_GetTicks64();
  while (true) {
    SDL_Event event;
    bool exit = false;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        exit = true;
        break;
      }
      ImGuiIO &io = ImGui::GetIO();
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (!io.WantCaptureMouse && !io.WantCaptureKeyboard) {
        this->mApplet->handle_event(*this, event);
      }
    }
    if (exit)
      break;

    uint64_t beginTime = SDL_GetTicks64();
    this->update((beginTime - prevTime) / 1000.0f);
    uint64_t endTime = SDL_GetTicks64();
    int32_t deltaTime = static_cast<int32_t>(endTime - beginTime);
    int32_t sleepTime = 16 - deltaTime;

    prevTime = beginTime;
    if (sleepTime > 0) {
      // This is not necessary as vsync will cap the framerate
      // SDL_Delay(sleepTime);
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
  if (this->mGLContext == NULL) {
    return 1;
  }
  SDL_GL_MakeCurrent(this->mWindow, this->mGLContext);
  // Try adaptive vsync, and use vsync if not supported
  if (SDL_GL_SetSwapInterval(-1) == -1) {
    SDL_GL_SetSwapInterval(1);
  }
  if (glewInit() != GLEW_OK) {
    return 1;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  ImGui_ImplSDL2_InitForOpenGL(this->mWindow, this->mGLContext);
  ImGui_ImplOpenGL3_Init();

  this->mApplet->init(*this);
  return 0;
}

void application::update(float pDelta) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
  ImGui::ShowDemoWindow();
  this->mApplet->update(*this, pDelta);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  SDL_GL_SwapWindow(this->mWindow);
}

void application::dispose() {
  this->mApplet->dispose();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(this->mGLContext);
  SDL_DestroyWindow(this->mWindow);
  SDL_Quit();
}

SDL_Window *&application::window() { return this->mWindow; }
