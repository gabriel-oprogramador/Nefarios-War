#pragma once
#include "CoreMinimal.h"

/*==================/ Input Manager /===================*/
ENGINE_API bool FIsKeyPressed(uchar KeyCode);
ENGINE_API bool FIsKeyRelease(uchar KeyCode);
ENGINE_API bool FIsKeyRepeat(uchar KeyCode);
ENGINE_API float FGetMouseX();
ENGINE_API float FGetMouseY();
ENGINE_API FVector2 FGetMousePos();
ENGINE_API void FSetMousePos(FVector2 NewPosition);
ENGINE_API void FCaptureMouse(bool bCapture);

/*==================/ Timer Manager /===================*/
ENGINE_API double FGetDeltaTime();
ENGINE_API uint32 FGetFrameRate();
ENGINE_API void FSetTargetFPS(uint32 Target);

/*==================/ Window Manager /===================*/
ENGINE_API void FInitWindow(int32 Width, int32 Height, cstring Title);
ENGINE_API void FCloseWindow();
ENGINE_API FVector2 FGetMonitorSize();
ENGINE_API FVector2 FGetWindowSize();
ENGINE_API void FSetWindowSize(FVector2 Size);
ENGINE_API void FSetWindowMode(EWindowMode Mode);
ENGINE_API EWindowMode FGetWindowMode();
ENGINE_API void FToggleFullscreen();
