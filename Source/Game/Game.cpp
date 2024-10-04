#include "GT/GameModule.h"

#include "GT/Renderer.h"
#include "GameFramework.h"

static FPrimitive SMesh;

void GameInit() {
  SMesh = FGetPrimitive(PS_CIRCLE);
}

void GameStart() {
}

void GameUpdate(float DeltaTime) {
  static FColor color = COLOR_RED;
  FDrawPrimitive(SMesh, color);

  //FVector2 pos = FGetMonitorSize();
  //GT_LOGTEMP(LOG_ALERT, "Mouse Pos => X:%f Y:%f", pos.x, pos.y);

  if(FIsKeyPressed(KEY_W)) {
    color = COLOR_YELLOW;
    FSetWindowSize((FVector2){1280, 768});
    //FSetWindowMode(WINDOW_MODE_RESIZABLE);
  }

  if(FIsKeyPressed(KEY_SPACE)) {
    static bool bCapture = false;
    bCapture = !bCapture;
    FCaptureMouse(bCapture);
  }

  if(FIsKeyPressed(KEY_S)) {
    color = COLOR_RED;
    FSetWindowSize((FVector2){800, 600});
    //FSetWindowMode(WINDOW_MODE_FIXED);
  }

  if(FIsKeyPressed(KEY_TAB)) {
    FVector2 size = FGetWindowSize();
    GT_LOGTEMP(LOG_ALERT, "Window Size => Width:%f Height:%f", size.x, size.y);
    FToggleFullscreen();
  }

  if(FIsKeyPressed(KEY_ESCAPE)) {
    FCloseWindow();
  }
}

void GameStop() {
}
