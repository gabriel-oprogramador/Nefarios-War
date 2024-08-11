#pragma once
#include "CoreMinimal.h"

#define CALL_API(Function, DefaultReturn,...) (Function != NULL) ? Function(__VA_ARGS__) : DefaultReturn;

typedef struct{
	Void (*WindowCreate)(Int32, Int32, String);
	Void (*WindowUpdate)();
	Void (*WindowDestroy)();
	Bool (*WindowShouldClose)();
} FWindowApi;

typedef struct{
	// Future implementation of the audio API!
} FAudioApi;

typedef struct{
	FWindowApi windowApi;
	FAudioApi audioApi;
// TODO:Refactor on the Linux platform!
	Void (*WindowCreate)(Int32, Int32, String);
	Void (*WindowUpdate)();
	Void (*WindowDestroy)();
	Bool (*WindowShouldClose)();
} FGT;

extern FGT GEngine;

Void EngineInit(Int32 Width, Int32 Height, String Title);
Bool EngineShouldClose();
Void EngineShutdown();
Void EngineBeginFrame();
Void EngineEndFrame();

// Moudules
Void* EngineLoadModule(String Name);
Void EngineFreeModule(Void* Module);
Void* EngineGetFunc(Void* Module, String Name);
Void EngineLoadApi(Void* Module, Void* Api, String* Names, Bool bDebugMode);

// Log
Void EnginePrintLog(ELogLevel Level, String Context, String Format, ...);
