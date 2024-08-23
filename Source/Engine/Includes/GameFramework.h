#pragma once
#include "CoreMinimal.h"

/*==================/ Input Manager /===================*/
Void FInputSetInputMode(EInputMode InputMode);
EInputMode FInputGetInputMode();

Bool FInputIsPressed(UChar KeyCode);
Bool FInputIsRelease(UChar KeyCode);
Bool FInputIsRepeat(UChar KeyCode);
Float FInputGetMouseX();
Float FInputGetMouseY();

Void FInputSetCursorPos(FVector2 NewPosition);
FVector2 FInputGetCursorPos();

Bool FInputIsCursorShow();
Void FInputShowCursor(Bool bShow);

Bool FInputIsCursorCaptured();
Void FInputCaptureCursor(Bool bCapture);

/*==================/ Timer Manager /===================*/
Double FTimerGetDeltaTime();
UInt32 FTimerGetFrameRate();
