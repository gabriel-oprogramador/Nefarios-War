#include "Game.h"

#include "GL/ApiGL.h"
#include "GameFramework.h"
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

  /*GT_LOGTEMP(LOG_INFO, "Timer => FPS:%d Ms:%f", GEngine.timerApi.frameRate, GEngine.timerApi.deltaTime);*/

  if(FInputIsPressed(KEY_ESCAPE)) {
    GEngine.windowApi.bShouldClose = true;
  }

  if(FInputIsPressed(KEY_TAB)) {
    GT_LOGTEMP(LOG_WARNING, "Toogle Fullscreen");
    static bool isFullscreen = false;
    isFullscreen = !isFullscreen;
    EngineSetFullscreen(isFullscreen);
  }

  if(FInputIsPressed(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Pressed:W");
  }
  if(FInputIsRelease(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Release:W");
  }
  if(FInputIsRepeat(KEY_W)) {
    /*GT_LOGTEMP(LOG_WARNING, "Key Reeat:W");*/
  }
}

// Called once at the end of the Game.
Void GameStop() {
}
