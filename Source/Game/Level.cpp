#include "Level.h"
#include "GL/ApiGL.h"
#include "GT/Engine.h"
#include "GameFramework.h"

static struct {
  FColor colorNormal;
  FColor colorFullscreen;
  Bool bFullscreen;
  Bool bControlState;
} Slevel;

Void LevelInit() {
  Slevel.bFullscreen = false;
  Slevel.bControlState = true;
  Slevel.colorNormal = COLOR_SKY_BLUE;
  Slevel.colorFullscreen = COLOR_YELLOW;
  glClearColor(FCOLOR_GL(Slevel.colorNormal));
}

Void LevelStart() {
}

Void LevelUpdate() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(FInputIsPressed(KEY_TAB)) {
    Slevel.bFullscreen = !Slevel.bFullscreen;
    EngineFullscreen(Slevel.bFullscreen);
    FColor color = (Slevel.bFullscreen) ? Slevel.colorFullscreen : Slevel.colorNormal;
    glClearColor(FCOLOR_GL(color));
  }

  if(FInputIsPressed(KEY_F1)){
    GT_LOGTEMP(LOG_WARNING, "Toogle Input Mode");
    EInputMode mode = (Slevel.bControlState) ? IM_GAME_ONLY : IM_UI_ONLY;
    Slevel.bControlState = !Slevel.bControlState;
    FInputSetMode(mode);
  }

  if(FInputIsPressed(KEY_ESCAPE)) {
    EngineShutdown();
  }
}
