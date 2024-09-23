#pragma once
#include "Level.h"

class UMainMenu : public ILevel{
public:
  UMainMenu();
  virtual void BeginPlay();
  virtual void Tick(float DeltaTime);
  virtual void EndPlay();
private:
  FColor m_colorNormal;
  FColor m_colorFullscreen;
  bool m_bFullscreen;
  bool m_bGameMode;
};
