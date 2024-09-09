#pragma once
#include "CoreMinimal.h"

class ILevel {
public:
  virtual Void BeginPlay() = 0;
  virtual Void Tick(Float DeltaTime) = 0;
  virtual Void EndPlay() = 0;
};

// In Game Open Level
template<typename TLevel>
TLevel* OpenLevel() {
  TLevel* lv = new TLevel();
  ((ILevel*)lv)->BeginPlay();
  return lv;
}
