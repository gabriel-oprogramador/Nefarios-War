#include "GameModule.h"
#include "Levels/MainMenu.h"

ILevel* SCurrentLevel = NULL;

Void GameInit() {
  SCurrentLevel = OpenLevel<UMainMenu>();
}

Void GameStart() {
}

Void GameUpdate(Float DeltaTime) {
  SCurrentLevel->Tick(DeltaTime);
}

Void GameStop() {
  SCurrentLevel->EndPlay();
}
