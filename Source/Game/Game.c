#include "Game.h"
#include "GameFramework.h"

// Called when starting the engine.
Void GameInit() {
}

// Called once at the beginning of the Game.
Void GameStart() {
}

// Called every frame.
Void GameUpdate(Float DeltaTime) {
  if (FInputIsPressed(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Pressed: W");
  }
  if (FInputIsRelease(KEY_W)) {
    GT_LOGTEMP(LOG_WARNING, "Key Release: W");
  }
}

// Called once at the end of the Game.
Void GameStop() {
}
