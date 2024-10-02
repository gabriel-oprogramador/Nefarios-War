#pragma once
#include "CoreMinimal.h"

typedef enum {
  ES_EDITOR_MODE = (1 << 0),
  ES_GAME_MODE = (1 << 1),
  ES_ENGINE_RUNNING = (1 << 2),
  ES_GAME_RUNNING = (1 << 3),
} EEngineState;

typedef struct {
  bool (*OnInitOpenGL)(int32, int32);
  bool (*OnCreateContext)();
  void (*OnDestroyContext)();
  void (*OnMakeCurrent)();
  void (*OnSwapBuffers)();
} IGraphicApi;

typedef struct {
  void (*OnCreate)(int32, int32, cstring, uint32);
  void (*OnUpdate)();
  void (*OnDestroy)();
  void (*OnSetFullscreen)(bool);
  void (*OnShowCursor)(bool);
  void (*OnSetSize)(int32, int32);
  void (*OnSetMode)(uint32);
  void (*OnSetCursorPos)(uint32, uint32);
  void (*OnSetCaptureCursor)(bool);
  bool (*OnIsFocused)();
} IWindowApi;

typedef struct {
  // Future implementation of the audio API!
  char* none;
} IAudioApi;

typedef struct {
  uchar previousKeys[KEY_MAX];
  uchar currentKeys[KEY_MAX];
  float mousePosition[2];
} IInputApi;

typedef struct {
  float frameTime;
  uint32 frameRate;
  double deltaTime;
  float engineStartTime;
} ITimeApi;

typedef struct {
  EEngineState state;
  FWindow mainWindow;
  IGraphicApi graphicApi;
  IWindowApi windowApi;
  IAudioApi audioApi;
  IInputApi inputApi;
  ITimeApi timerApi;
} FGT;

ENGINE_API FGT GEngine;

ENGINE_API bool FEngineInitialize(uint64 Flags, const char** Args);
ENGINE_API void FEngineMainLoop();
ENGINE_API void FEngineTerminate();
ENGINE_API void FEngineShutdown();
ENGINE_API EEngineState FEngineGetState();
