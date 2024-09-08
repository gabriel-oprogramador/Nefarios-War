#include "GameModule.h"
#include "Level.h"

Void GameInit() {
  LevelInit();
}

Void GameStart() {
  LevelStart();
}

Void GameUpdate(Float DeltaTime) {
  LevelUpdate();
}

Void GameStop() {
}
