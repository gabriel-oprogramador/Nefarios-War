#pragma once
#include "CoreMinimal.h"

/*==================/ Input Manager /===================*/
ENGINE_API void FInputSetMode(EInputMode InputMode);
ENGINE_API EInputMode FInputGetMode();

ENGINE_API bool FInputIsPressed(uchar KeyCode);
ENGINE_API bool FInputIsRelease(uchar KeyCode);
ENGINE_API bool FInputIsRepeat(uchar KeyCode);
ENGINE_API float FInputGetMouseX();
ENGINE_API float FInputGetMouseY();

ENGINE_API void FInputSetCursorPos(FVector2 NewPosition);
ENGINE_API FVector2 FInputGetCursorPos();

ENGINE_API bool FInputIsCursorShow();
ENGINE_API void FInputShowCursor(bool bShow);

ENGINE_API bool FInputIsCursorCaptured();
ENGINE_API void FInputCaptureCursor(bool bCapture);

/*==================/ Timer Manager /===================*/
ENGINE_API double FTimerGetDeltaTime();
ENGINE_API uint32 FTimerGetFrameRate();
