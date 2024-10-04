#include "GT/Engine.h"
#include "GT/Renderer.h"
#include "GameFramework.h"

// Engine Entrypoint
int GTmain(int argc, const char** argv) {
  const float ASPECT = 1.7777;
  const int32 WIDTH = 800;
  const int32 HEIGHT = WIDTH / ASPECT;

  FEngineInitialize(0, argv);
  FSetTargetFPS(60);

  FInitWindow(WIDTH, HEIGHT, STR(GAME_NAME));
  FSetClearColor(COLOR_WHITE);

  FEngineMainLoop();

  FCloseWindow();
  FEngineTerminate();
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
