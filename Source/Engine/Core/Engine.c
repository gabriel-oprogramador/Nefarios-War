#include "GT/Engine.h"
#include "GT/Platform.h"
#include "GT/Renderer.h"
#include "GT/GameModule.h"

FGT GEngine;

static void InternalBeginFrame();
static void InternalEndFrame();
static double SFrameStartTime = 0;

bool FEngineInitialize(uint64 Flags, const char** Args) {
  PPlatformInitialize();
  FRendererInitialize(OPENGL_VERSION_3_3);
  GEngine.state = (ES_ENGINE_RUNNING | ES_GAME_MODE);
  GameInit();
  GEngine.timerApi.engineStartTime = PGetTime();
  GT_LOG(LOG_INFO, "API-GT: Initialized");
  return true;
}

void FEngineMainLoop() {
  if(GEngine.state & ES_GAME_MODE) {
    GEngine.state |= ES_GAME_RUNNING;
    GameStart();
  }
  while(FEngineGetState() & ES_ENGINE_RUNNING) {
    InternalBeginFrame();
    GEngine.windowApi.OnUpdate();
    if(GEngine.state & ES_GAME_RUNNING) {
      GameUpdate(GEngine.timerApi.deltaTime);
    }

    InternalEndFrame();
  }
}

void FEngineTerminate() {
  if(GEngine.state & ES_GAME_RUNNING) {
    GameStop();
  }
  FRendererTerminate();
  PPlatformTerminate();
  GT_LOG(LOG_INFO, "API-GT: Terminated");
}

void FEngineShutdown() {
  GEngine.state &= ~ES_ENGINE_RUNNING;
}

EEngineState FEngineGetState() {
  return GEngine.state;
}

// Internal Functions
static void InternalBeginFrame() {
  SFrameStartTime = PGetTime();
  if(GEngine.mainWindow.bCursorCaptured) {
    GEngine.windowApi.OnSetCursorPos(GEngine.mainWindow.width / 2, GEngine.mainWindow.height / 2);
  }
  FClearBuffers();
}

static void InternalEndFrame() {
  GEngine.graphicApi.OnSwapBuffers();
  double end = PGetTime();
  double delta = end - SFrameStartTime;
  double target = GEngine.timerApi.frameTime;
  double remainingTime = target - delta;

  if(target > 0) {
    if(remainingTime > (delta * 0.9)) {
      PWait(remainingTime);
    }
    while((delta = PGetTime() - SFrameStartTime) < target) {}
  }

  GEngine.timerApi.deltaTime = delta;
  GEngine.timerApi.frameRate = (uint32)ceil(1.f / delta);
}
