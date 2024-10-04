#include "GameFramework.h"
#include "GT/Engine.h"
#include "GT/Renderer.h"

void FInitWindow(int32 Width, int32 Height, cstring Title) {
  if(GEngine.mainWindow.pWindow != NULL) {
    return;
  }
  uint32 mode = WINDOW_MODE_FIXED;
  GEngine.windowApi.OnCreate(Width, Height, Title, mode);
  GEngine.graphicApi.OnCreateContext();
  GEngine.graphicApi.OnMakeCurrent();
  int32 windowWidth = GEngine.mainWindow.width;
  int32 windowHeight = GEngine.mainWindow.height;
  FSetViewport((FRect){0, 0, windowWidth, windowHeight});
  GT_LOG(LOG_INFO, "API-GT: Created Window => Width:%d Height:%d Title:%s", windowWidth, windowHeight, Title);
}

void FCloseWindow() {
  if(GEngine.mainWindow.pWindow == NULL){
    return;
  }
  GEngine.windowApi.OnDestroy();
  if(GEngine.mainWindow.pContext){
    GEngine.graphicApi.OnDestroyContext();
  }
  FEngineShutdown();
}

FVector2 FGetMonitorSize() {
  return (FVector2){GEngine.mainWindow.monitorWidth, GEngine.mainWindow.monitorHeight};
}

FVector2 FGetWindowSize() {
  return (FVector2){GEngine.mainWindow.width, GEngine.mainWindow.height};
}

void FSetWindowSize(FVector2 Size) {
  GEngine.windowApi.OnSetSize(Size.x, Size.y);
}

void FSetWindowMode(EWindowMode Mode) {
  GEngine.mainWindow.windowMode = Mode;
  GEngine.windowApi.OnSetMode(Mode);
}

EWindowMode FGetWindowMode() {
  return GEngine.mainWindow.windowMode;
}

void FToggleFullscreen() {
  bool bFullscreen = !GEngine.mainWindow.bFullscreen;
  GEngine.windowApi.OnSetFullscreen(bFullscreen);
}
