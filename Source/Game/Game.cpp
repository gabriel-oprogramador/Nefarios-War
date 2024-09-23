#include "GameModule.h"
#include "Levels/MainMenu.h"

ILevel* SCurrentLevel = NULL;

void GameInit() {
  SCurrentLevel = OpenLevel<UMainMenu>();
}

void GameStart() {
}

void GameUpdate(float DeltaTime) {
  SCurrentLevel->Tick(DeltaTime);
}

void GameStop() {
  SCurrentLevel->EndPlay();
}
