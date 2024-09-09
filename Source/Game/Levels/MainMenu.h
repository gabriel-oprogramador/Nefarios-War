#pragma once
#include "Level.h"

class UMainMenu : public ILevel{
public:
  UMainMenu();
  virtual Void BeginPlay();
  virtual Void Tick(Float DeltTime);
  virtual Void EndPlay();
private:
  FColor m_colorNormal;
  FColor m_colorFullscreen;
  Bool m_bFullscreen;
  Bool m_bGameMode;
};
