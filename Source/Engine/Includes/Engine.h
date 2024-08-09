#pragma once
#include "CoreMinimal.h"

Void EngineInit(Int32 Width, Int32 Height, String Title);
Bool EngineShouldClose();
Void EngineShutdown();
Void EngineBeginFrame();
Void EngineEndFrame();
Void EnginePrintLog(ELogLevel Level, String Module, String Context, String Format, ...);
