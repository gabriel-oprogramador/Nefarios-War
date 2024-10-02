#include "GameFramework.h"
#include "GT/Engine.h"

bool FIsKeyPressed(uchar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && !GEngine.inputApi.previousKeys[KeyCode]);
}

bool FIsKeyRelease(uchar KeyCode) {
  return (!GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}

bool FIsKeyRepeat(uchar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}

void FCaptureMouse(bool bCapture) {
  GEngine.mainWindow.bCursorCaptured = bCapture;
  GEngine.windowApi.OnShowCursor(!bCapture);
}

float FGetMouseX() {
  return GEngine.inputApi.mousePosition[0];
}

float FGetMouseY() {
  return GEngine.inputApi.mousePosition[1];
}

FVector2 FGetMousePos() {
  return (FVector2){GEngine.inputApi.mousePosition[0], GEngine.inputApi.mousePosition[1]};
}

void FSetMousePos(FVector2 NewPosition) {
  GEngine.windowApi.OnSetCursorPos((uint32)NewPosition.x, (uint32)NewPosition.y);
}
