#include "GameFramework.h"
#include "GT/Engine.h"

void FTimerUpdate(float DeltaTime) {
  // Call timers update
}

double FTimerGetDeltaTime() {
  return GEngine.timerApi.deltaTime;
}

uint32 FTimerGetFrameRate() {
  return GEngine.timerApi.frameRate;
}
