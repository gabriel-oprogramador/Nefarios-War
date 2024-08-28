/*#include "Game.h"*/
#include "Engine.h"
#include "GameFramework.h"
#include "GL/ApiGL.h"

static struct{
  FColor colorNormal;
  FColor colorFullscreen;
} SGame;

// Called when starting the engine.
Void GameInit() {
  SGame.colorNormal = COLOR_YELLOW;
  SGame.colorFullscreen = COLOR_PURPLE_DARK;
  glClearColor(FCOLOR_GL(COLOR_YELLOW));
}

// Called once at the beginning of the Game.
Void GameStart() {
}

// Called every frame.
Void GameUpdate(Float DeltaTime) {
  // TODO:To Test OpenGL
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /*Low Level Calls by GEngine..*/
  // GT_LOGTEMP(LOG_INFO, "Timer => FPS:%u Ms:%f", GEngine.timerApi.frameRate, GEngine.timerApi.deltaTime);

  /*High Level Calls via GameFramework.*/
  // GT_LOGTEMP(LOG_INFO, "Timer => FPS:%u Ms:%f", FTimerGetFrameRate(), FTimerGetDeltaTime());

  if(FInputIsPressed(KEY_F1)) {
    static Bool bShow = false;
    bShow = !bShow;
    if(bShow) {
      FInputSetMode(IM_GAME_ONLY);
      GT_LOGTEMP(LOG_WARNING, "Input Mode => Game Only");
    } else {
      FInputSetMode(IM_UI_ONLY);
      GT_LOGTEMP(LOG_WARNING, "Input Mode => UI Only");
    }
  }

  if(FInputIsPressed(KEY_ESCAPE)) {
    GEngine.windowApi.bShouldClose = true;
  }

  if(FInputIsPressed(KEY_TAB)) {
    GT_LOGTEMP(LOG_WARNING, "Toogle Fullscreen");
    static bool isFullscreen = false;
    isFullscreen = !isFullscreen;
    EngineFullscreen(isFullscreen);
    FColor color = (isFullscreen) ? SGame.colorFullscreen : SGame.colorNormal;
    glClearColor(FCOLOR_GL(color));
  }

  if(FInputIsPressed(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Pressed:W");
  }
  if(FInputIsRelease(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Release:W");
  }
  if(FInputIsRepeat(KEY_W)) {
    /*GT_LOGTEMP(LOG_WARNING, "Key Repeat:W");*/
  }
}

// Called once at the end of the Game.
Void GameStop() {
}
