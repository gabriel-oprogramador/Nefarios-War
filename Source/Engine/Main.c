#include "GT/Engine.h"
#include "GameModule.h"
#include "Renderer.h"

// Engine primary systems.
extern void FInputUpdate(float DeltaTime);
extern void FTimerUpdate(float DeltaTime);

// Gameplay Control.
static bool SbInitGamePlay = true;
static bool SbGameIsRunning = false;
static float deltaTime = 0.f;

// Engine Entrypoint
int GTmain(int argc, const char** argv) {
  PEngineProcess(0, argv);
  PEngineInitialize(800, 600, STR(GAME_NAME));
  RRendererInitialize();

  PEngineSetTargetFPS(60);
  GameInit();

  while(!PEngineShouldClose()) {
    deltaTime = (float)GEngine.timerApi.deltaTime;
    PEngineBeginFrame();
    FInputUpdate(deltaTime);
    if(SbInitGamePlay && !SbGameIsRunning) {
      SbGameIsRunning = true;
      GameStart();
    }
    if(SbInitGamePlay) {
      FTimerUpdate(deltaTime);
      GameUpdate(deltaTime);
    }
    PEngineEndFrame();
  }
  GameStop();
  RRendererTerminate();
  PEngineTerminate();

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
