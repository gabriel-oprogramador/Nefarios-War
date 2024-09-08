#pragma once
#include "CoreMinimal.h"

/*==================/ Input Manager /===================*/
ENGINE_API Void FInputSetMode(EInputMode InputMode);
ENGINE_API EInputMode FInputGetMode();

ENGINE_API Bool FInputIsPressed(UChar KeyCode);
ENGINE_API Bool FInputIsRelease(UChar KeyCode);
ENGINE_API Bool FInputIsRepeat(UChar KeyCode);
ENGINE_API Float FInputGetMouseX();
ENGINE_API Float FInputGetMouseY();

ENGINE_API Void FInputSetCursorPos(FVector2 NewPosition);
ENGINE_API FVector2 FInputGetCursorPos();

ENGINE_API Bool FInputIsCursorShow();
ENGINE_API Void FInputShowCursor(Bool bShow);

ENGINE_API Bool FInputIsCursorCaptured();
ENGINE_API Void FInputCaptureCursor(Bool bCapture);

/*==================/ Timer Manager /===================*/
ENGINE_API Double FTimerGetDeltaTime();
ENGINE_API UInt32 FTimerGetFrameRate();
