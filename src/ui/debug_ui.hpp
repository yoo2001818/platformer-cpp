#ifndef __DEBUG_UI_HPP__
#define __DEBUG_UI_HPP__

namespace platformer {
class game;
class debug_ui_system {
public:
  debug_ui_system();
  void init(game &pGame);
  void update(game &pGame, float pDelta);

private:
  game *mGame;
  int mSceneIndex;
  float mDeltaTime;
  int mDeltaFrames;
  float mFps;
};
}; // namespace platformer

#endif // __DEBUG_UI_HPP__
