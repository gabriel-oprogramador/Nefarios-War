#include "MainMenu.h"
#include "GT/Engine.h"
#include "GameFramework.h"
#include "GL/ApiGL.h"

UMainMenu::UMainMenu() {
  this->m_colorNormal = COLOR_SKY_BLUE;
  this->m_colorFullscreen = COLOR_YELLOW;
  this->m_bFullscreen = false;
  this->m_bGameMode = false;
  glClearColor(FCOLOR_GL(this->m_colorNormal));
}

Void UMainMenu::BeginPlay() {
  
}

Void UMainMenu::Tick(Float DeltTime) {
  if(FInputIsPressed(KEY_TAB)){
    this->m_bFullscreen = !this->m_bFullscreen;
    EngineFullscreen(this->m_bFullscreen);
    FColor color = (this->m_bFullscreen) ? this->m_colorFullscreen : this->m_colorNormal;
    glClearColor(FCOLOR_GL(color));
  }

  if(FInputIsPressed(KEY_F1)){
    this->m_bGameMode = !this->m_bGameMode;
    EInputMode mode = (this->m_bGameMode) ? IM_GAME_ONLY : IM_UI_ONLY;
    FInputSetMode(mode);
  }

  if(FInputIsPressed(KEY_ESCAPE)){
    EngineShutdown();
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

Void UMainMenu::EndPlay() {
}
