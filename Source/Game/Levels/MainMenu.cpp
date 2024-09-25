#include "MainMenu.h"
#include "GT/Engine.h"
#include "GameFramework.h"
#include "Renderer.h"

UMainMenu::UMainMenu() {
  this->m_colorNormal = COLOR_WHITE;
  this->m_colorFullscreen = COLOR_YELLOW;
  this->m_bFullscreen = false;
  this->m_bGameMode = false;
  this->quad = RGetPrimitive(PS_CIRCLE);
  RSetClearColor(this->m_colorNormal);
}

void UMainMenu::BeginPlay() {
}

void UMainMenu::Tick(float DeltTime) {
  //TODO:Draw First Primitive
  float w = GEngine.windowApi.width;
  float h = GEngine.windowApi.height;
  RSetViewport((FRect){0, 0, w, h});
  RDrawPrimitive(this->quad, COLOR_RED);

  if(FInputIsPressed(KEY_TAB)) {
    this->m_bFullscreen = !this->m_bFullscreen;
    PEngineFullscreen(this->m_bFullscreen);
    FColor color = (this->m_bFullscreen) ? this->m_colorFullscreen : this->m_colorNormal;
    RSetClearColor(color);
  }

  if(FInputIsPressed(KEY_F1)) {
    this->m_bGameMode = !this->m_bGameMode;
    EInputMode mode = (this->m_bGameMode) ? IM_GAME_ONLY : IM_UI_ONLY;
    FInputSetMode(mode);
  }

  if(FInputIsPressed(KEY_ESCAPE)) {
    PEngineShutdown();
  }
}

void UMainMenu::EndPlay() {
}
