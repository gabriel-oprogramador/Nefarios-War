#include "Engine.h"
#include "GameFramework.h"

Bool FInputIsPressed(UChar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && !GEngine.inputApi.previousKeys[KeyCode]);
}

Bool FInputIsRelease(UChar KeyCode) {
  return (!GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}

Bool FInputIsRepeat(UChar KeyCode) {
  return (GEngine.inputApi.currentKeys[KeyCode] && GEngine.inputApi.previousKeys[KeyCode]);
}
