#pragma once
#include "CoreMinimal.h"

typedef struct {
  int32 width;
  int32 height;
  cstring title;
  bool bFullscreen;
  bool bShouldClose;
  bool bShowCursor;
  bool bCursorCaptured;
  void (*OnWindowCreate)(int32, int32, cstring);
  void (*OnWindowUpdate)();
  void (*OnWindowDestroy)();
  void (*OnWindowFullscreen)(bool);
  void (*OnWindowShowCursor)(bool);
  void (*OnWindowSetCursorPos)(uint32, uint32);
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
  IWindowApi windowApi;
  IAudioApi audioApi;
  IInputApi inputApi;
  ITimeApi timerApi;
} FGT;

ENGINE_API FGT GEngine;

ENGINE_API bool EngineProcess(uint64 Flags, cstring* Args);
ENGINE_API void EngineInitialize(int32 Width, int32 Height, cstring Title);
ENGINE_API void EngineTerminate();
ENGINE_API bool EngineShouldClose();
ENGINE_API void EngineShutdown();
ENGINE_API void EngineBeginFrame();
ENGINE_API void EngineEndFrame();
ENGINE_API void EngineFullscreen(bool bFullscreen);

// Returns in Seconds
ENGINE_API double EngineGetTime();
ENGINE_API void EngineSetTargetFPS(uint32 Target);
ENGINE_API void EngineWait(double Milliseconds);

// Module
ENGINE_API void* EngineLoadModule(cstring Name);
ENGINE_API void EngineFreeModule(void* Module);
ENGINE_API void* EngineGetFunc(void* Module, cstring Name);
ENGINE_API void EngineLoadApi(void* Module, void* Api, cstring* Names, bool bDebugMode);

#define MOUSE_LEFT_CODE     0b0001  // 0x1 D1
#define MOUSE_MIDDLE_CODE   0b0010  // 0x2 D2
#define MOUSE_RIGHT_CODE    0b0011  // 0x3 D3
#define MOUSE_FORWARD_CODE  0b0100  // 0x4 D4
#define MOUSE_BACKWARD_CODE 0b0101  // 0x5 D5
