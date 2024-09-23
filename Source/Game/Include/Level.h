#pragma once
#include "CoreMinimal.h"

class ILevel {
public:
  virtual void BeginPlay() = 0;
  virtual void Tick(float DeltaTime) = 0;
  virtual void EndPlay() = 0;
};

// In Game Open Level
template<typename TLevel>
TLevel* OpenLevel() {
  TLevel* lv = new TLevel();
  ((ILevel*)lv)->BeginPlay();
  return lv;
}
