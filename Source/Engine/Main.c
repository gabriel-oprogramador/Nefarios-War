#include "Engine.h"

// Game Entrypoint
extern Void GameInit();
extern Void GameStart();
extern Void GameUpdate(Float DeltaTime);
extern Void GameStop();

// Engine Entrypoint
int GTmain(int argc, const char** argv) {
  EngineInit(1280, 768, STR(GAME_NAME));

  GT_LOGTEMP(LOG_WARNING, "My Temp Log:%d", 27);
  GT_LOG(LOG_INFO, "Init Game %s", STR(GAME_NAME));
  GT_LOG(LOG_INFO, "Config Path => %s", FIND_CONFIG("Engine/Settings.txt"));
  GT_LOG(LOG_INFO, "Asset Path => %s", FIND_ASSET("Texture/PlayerShip.bmp"));

  Int32 number = 27;
  GT_LOG(LOG_INFO, "My Log System:%d", number);
  GT_LOG(LOG_SUCCESS, "My Log System:%d", number);
  GT_LOG(LOG_WARNING, "My Log System:%d", number);
  GT_LOG(LOG_ERROR, "My Log System:%d", number);
  GT_LOG(LOG_FATAL, "My Log System:%d", number);

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
