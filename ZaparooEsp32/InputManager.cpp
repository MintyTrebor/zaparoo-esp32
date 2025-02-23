#include "ZaparooEsp32.hpp"
#include "InputManager.h"

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

//Prefences has a 14 character limit for key
void InputManager::init(ScreenManager* scrnMgr){
  screenManager = scrnMgr;
}

void InputManager::nextRotation(){
  if(lastRotationPos == 2){
    lastRotationPos = 0;
  } 
  else {
    lastRotationPos++;
  }
  if(lastRotationPos == 0){
    screenManager->dispInsCoin();
  }
  if(lastRotationPos == 1){
    screenManager->disp1PStart();
  }
  if(lastRotationPos == 2){
    screenManager->disp2PStart();
  }
}