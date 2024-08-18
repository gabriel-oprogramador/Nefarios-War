#pragma once
#include "CoreMinimal.h"

#define CALL_API(Function, DefaultReturn, ...) (Function != NULL) ? Function(__VA_ARGS__) : DefaultReturn;

typedef struct {
  Int32 width;
  Int32 height;
  String title;
  Bool bFullsreen;
  Bool bShouldClose;
  Void (*OnWindowCreate)(Int32, Int32, String);
  Void (*OnWindowUpdate)();
  Void (*OnWindowDestroy)();
  Void (*OnWindowFullscreen)(Bool);
} FWindowApi;

typedef struct {
  // Future implementation of the audio API!
} FAudioApi;

typedef struct {
  UChar previousKeys[KEY_MAX];
  UChar currentKeys[KEY_MAX];
  Float mousePosition[2];
} FInputApi;

typedef struct {
  FWindowApi windowApi;
  FAudioApi audioApi;
  FInputApi inputApi;
  Float deltaTime;
  Float frameTime;
} FGT;

extern FGT GEngine;

Void EngineInit(Int32 Width, Int32 Height, String Title);
Bool EngineShouldClose();
Void EngineShutdown();
Void EngineBeginFrame();
Void EngineEndFrame();
Void EngineFullscreen(Bool bIsFullscreen);

Void EngineSetTargetFPS(UInt32 Target);
Float EngineGetTime();
Void EngineWait(Float Time);

// Graphic
Bool EngineInitGraphic();


// Module
Void* EngineLoadModule(String Name);
Void EngineFreeModule(Void* Module);
Void* EngineGetFunc(Void* Module, String Name);
Void EngineLoadApi(Void* Module, Void* Api, String* Names, Bool bDebugMode);

// Log
Void EnginePrintLog(ELogLevel Level, String Context, String Format, ...);

#define MOUSE_LEFT_CODE     0b0001 // 0x1 D1
#define MOUSE_MIDDLE_CODE   0b0010 // 0x2 D2
#define MOUSE_RIGHT_CODE    0b0011 // 0x3 D3
#define MOUSE_FORWARD_CODE  0b0100 // 0x4 D4
#define MOUSE_BACKWARD_CODE 0b0101 // 0x5 D5
