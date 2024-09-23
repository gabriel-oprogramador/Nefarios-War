#include "GameFramework.h"
#include "GT/Engine.h"

static EInputMode SInputMode;

void FInputUpdate(float DeltaTime) {
  if(GEngine.windowApi.bCursorCaptured) {
    FInputSetCursorPos((FVector2){GEngine.windowApi.width / 2.f, GEngine.windowApi.height / 2.f});
  }

  switch(SInputMode) {
    case IM_GAME_ONLY: {
      // Calls only Game Events
    } break;
    case IM_UI_ONLY: {
      // Calls only UI Events
    } break;
    case IM_GAME_UI: {
      // Calls only Game and UI Events
    } break;
    case IM_EDITOR_ONLY: {
      // Calls only Editor Events
    } break;
  }
}

EInputMode FInputGetMode() {
  return SInputMode;
}

void FInputSetMode(EInputMode InputMode) {
  SInputMode = InputMode;
  switch(InputMode) {
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
    case IM_EDITOR_ONLY: {
    } break;
  }
}

bool FInputIsPressed(uchar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && !GEngine.inputApi.previousKeys[KeyCode]);
}

bool FInputIsRelease(uchar KeyCode) {
  return (!GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}

bool FInputIsRepeat(uchar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}

float FInputGetMouseX() {
  return GEngine.inputApi.mousePosition[0];
}

float FInputGetMouseY() {
  return GEngine.inputApi.mousePosition[1];
}

void FInputSetCursorPos(FVector2 NewPosition) {
  GEngine.windowApi.OnWindowSetCursorPos((uint32)NewPosition.x, (uint32)NewPosition.y);
}

FVector2 FInputGetCursorPos() {
  return (FVector2){GEngine.inputApi.mousePosition[0], GEngine.inputApi.mousePosition[1]};
}

bool FInputIsCursorShow() {
  return GEngine.windowApi.bShowCursor;
}

void FInputShowCursor(bool bShow) {
  GEngine.windowApi.OnWindowShowCursor(bShow);
}

bool FInputIsCursorCaptured() {
  return GEngine.windowApi.bCursorCaptured;
}

void FInputCaptureCursor(bool bCapture) {
  GEngine.windowApi.bCursorCaptured = bCapture;
}
