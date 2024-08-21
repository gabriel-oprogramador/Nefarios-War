#include "Engine.h"
#include "GameFramework.h"

// Game Entrypoint
extern Void GameInit();
extern Void GameStart();
extern Void GameUpdate(Float DeltaTime);
extern Void GameStop();

static Bool SbInitGamePlay = true;
static Bool SbGameIsRunning = false;
static Float deltaTime = 0.f;

// Engine Entrypoint
int GTmain(int argc, const char** argv) {
  EngineInit(800, 600, STR(GAME_NAME));
  EngineSetTargetFPS(60);
  GameInit();

  while(!EngineShouldClose()) {
    deltaTime = GEngine.timerApi.deltaTime;

    EngineBeginFrame();
    if(SbInitGamePlay && !SbGameIsRunning) {
      SbGameIsRunning = true;
      GameStart();
    }
    if(SbInitGamePlay) {
      FInputUpdate(deltaTime);
      GameUpdate(deltaTime);
    }
    EngineEndFrame();
  }
  GameStop();
  EngineShutdown();

  return 0;
}

// Platform Entrypoint
#if defined(PLATFORM_WINDOWS) && defined(RELEASE_MODE)
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  printf("Entrypoint => WinMain\n");
  return GTmain(0, (const char**)lpCmdLine);
}
#elif defined(PLATFORM_LINUX) || defined(DEBUG_MODE)
int main(int argc, const char** argv) {
  printf("Entrypoint => Main\n");
  return GTmain(argc, argv);
}
#endif
