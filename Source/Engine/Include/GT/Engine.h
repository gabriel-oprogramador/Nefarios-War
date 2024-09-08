#pragma once
#include "CoreMinimal.h"

typedef struct {
  Int32 width;
  Int32 height;
  String title;
  Bool bFullscreen;
  Bool bShouldClose;
  Bool bShowCursor;
  Bool bCursorCaptured;
  Void (*OnWindowCreate)(Int32, Int32, String);
  Void (*OnWindowUpdate)();
  Void (*OnWindowDestroy)();
  Void (*OnWindowFullscreen)(Bool);
  Void (*OnWindowShowCursor)(Bool);
  Void (*OnWindowSetCursorPos)(UInt32, UInt32);
} IWindowApi;

typedef struct {
  // Future implementation of the audio API!
  Char* none;
} IAudioApi;

typedef struct {
  UChar previousKeys[KEY_MAX];
  UChar currentKeys[KEY_MAX];
  Float mousePosition[2];
} IInputApi;

typedef struct {
  Float frameTime;
  UInt32 frameRate;
  Double deltaTime;
  Float engineStartTime;
} ITimeApi;

typedef struct {
  IWindowApi windowApi;
  IAudioApi audioApi;
  IInputApi inputApi;
  ITimeApi timerApi;
} FGT;

ENGINE_API FGT GEngine;

ENGINE_API Void EngineInitialize(Int32 Width, Int32 Height, String Title);
ENGINE_API Void EngineTerminate();
ENGINE_API Bool EngineShouldClose();
ENGINE_API Void EngineShutdown();
ENGINE_API Void EngineBeginFrame();
ENGINE_API Void EngineEndFrame();
ENGINE_API Void EngineFullscreen(Bool bFullscreen);

// Returns in Seconds
ENGINE_API Double EngineGetTime();
ENGINE_API Void EngineSetTargetFPS(UInt32 Target);
ENGINE_API Void EngineWait(Double Milliseconds);

// Module
ENGINE_API Void* EngineLoadModule(String Name);
ENGINE_API Void EngineFreeModule(Void* Module);
ENGINE_API Void* EngineGetFunc(Void* Module, String Name);
ENGINE_API Void EngineLoadApi(Void* Module, Void* Api, String* Names, Bool bDebugMode);

#define MOUSE_LEFT_CODE     0b0001  // 0x1 D1
#define MOUSE_MIDDLE_CODE   0b0010  // 0x2 D2
#define MOUSE_RIGHT_CODE    0b0011  // 0x3 D3
#define MOUSE_FORWARD_CODE  0b0100  // 0x4 D4
#define MOUSE_BACKWARD_CODE 0b0101  // 0x5 D5
