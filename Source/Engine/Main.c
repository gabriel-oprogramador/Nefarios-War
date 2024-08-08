#include "CoreMinimal.h"

// Game Entrypoint
extern Void GameInit();
extern Void GameStart();
extern Void GameUpdate(Float DeltaTime);
extern Void GameStop();

// Engine Entrypoint
int GTmain(int argc, const char** argv){
	printf("Init Game %s\n", STR(GAME_NAME));
	printf("Config Path => %s\n", FIND_CONFIG("Engine/Settings.txt"));
	printf("Asset Path => %s\n", FIND_ASSET("Texture/PlayerShip.bmp"));
	return 0;
}

// Platform Entrypoint
#if defined(PLATFORM_WINDOWS) && defined(RELEASE_MODE)
#include <windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
	printf("Entrypoint => WinMain\n");
	return GTmain(0, (const char**)lpCmdLine);
}
#elif defined(PLATFORM_LINUX) || defined(DEBUG_MODE)
int main(int argc, const char** argv){
	printf("Entrypoint => Main\n");
	return GTmain(argc, argv);
}
#endif
