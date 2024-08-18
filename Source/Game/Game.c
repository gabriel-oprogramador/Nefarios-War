#include "Game.h"

#include "GameFramework.h"
#include "GL/ApiGL.h"
#include "Engine.h"

// Called when starting the engine.
Void GameInit() {
}

// Called once at the beginning of the Game.
Void GameStart() {
}

// Called every frame.
Void GameUpdate(Float DeltaTime) {
  // TODO:To Test OpenGL
  GL.glClearColor(1.f, 1.f, 0.f, 1.f);
  GL.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(FInputIsPressed(KEY_TAB)){
    GT_LOGTEMP(LOG_WARNING, "Toogle Fullscreen");
    static bool isFullscree = false;
    isFullscree = !isFullscree;
    EngineFullscreen(isFullscree);
  }

  if (FInputIsPressed(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Pressed:W");
  }
  if (FInputIsRelease(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Release:W");
  }
  if (FInputIsRepeat(KEY_W)) {
    /*GT_LOGTEMP(LOG_WARNING, "Key Reeat:W");*/
  }
}

// Called once at the end of the Game.
Void GameStop() {
}
