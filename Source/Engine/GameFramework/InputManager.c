#include "Engine.h"
#include "GameFramework.h"

static EInputMode SInputMode;

Void FInputUpdate(Float DeltaTime) {
  if (GEngine.windowApi.bCursorCaptured) {
    FInputSetCursorPos((FVector2){GEngine.windowApi.width / 2.f, GEngine.windowApi.height / 2.f});
  }
}

EInputMode FInputGetInputMode() {
  return SInputMode;
}
Void FInputSetInputMode(EInputMode InputMode) {
  SInputMode = InputMode;
  switch (InputMode) {
    case IM_GAME_ONLY: {
      FInputShowCursor(false);
      FInputCaptureCursor(true);
    } break;
    case IM_UI_ONLY: {
      FInputShowCursor(true);
      FInputCaptureCursor(false);
    } break;
    case IM_GAME_UI: {
      FInputShowCursor(true);
      FInputCaptureCursor(false);
    } break;
  }
}

Bool FInputIsPressed(UChar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && !GEngine.inputApi.previousKeys[KeyCode]);
}

Bool FInputIsRelease(UChar KeyCode) {
  return (!GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}

Bool FInputIsRepeat(UChar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}

Float FInputGetMouseX() {
  return GEngine.inputApi.mousePosition[0];
}

Float FInputGetMouseY() {
  return GEngine.inputApi.mousePosition[1];
}

Void FInputSetCursorPos(FVector2 NewPosition) {
  CALL_API(GEngine.windowApi.OnWindowSetCursorPos, NULL, NewPosition.x, NewPosition.y);
}

FVector2 FInputGetCursorPos() {
  return (FVector2){GEngine.inputApi.mousePosition[0], GEngine.inputApi.mousePosition[1]};
}

Bool FInputIsCursorShow() {
  return GEngine.windowApi.bShowCursor;
}

Void FInputShowCursor(Bool bShow) {
  CALL_API(GEngine.windowApi.OnWindowShowCursor, NULL, bShow);
}

Bool FInputIsCursorCaptured() {
  return GEngine.windowApi.bCursorCaptured;
}

Void FInputCaptureCursor(Bool bCapture) {
  GEngine.windowApi.bCursorCaptured = bCapture;
}
