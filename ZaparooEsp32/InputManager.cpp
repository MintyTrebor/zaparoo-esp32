#include "ZaparooEsp32.hpp"
#include "InputManager.h"

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

void InputManager::init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan){
  screenManager = scrnMgr;
  encoder = encdr;
  feedbackManager = fbMan;
}

void InputManager::doRotaryButton(){
  if(lastScrnPos == 0){
    Serial.println("Do Insert Coin Action");
  }
  if(lastScrnPos == 1){
    Serial.println("Do 1Player Start Action");
  }
  if(lastScrnPos == 2){
    Serial.println("Do 2Player Start Action");
  }
}
void InputManager::doRotaryTurn(){
  int currRotPos = encoder->getPosition();
  if(lastRotationPos != currRotPos){
    nextRotation();
    lastRotationPos = currRotPos;
    encoder->tick();
  }else{
    encoder->tick();
  }
  //Serial.println(String(currRotPos));
}

void InputManager::nextRotation(){
  if(lastScrnPos == 3){
    lastScrnPos = 0;
  } 
  else {
    lastScrnPos++;
  }
  if(lastScrnPos == 0){
    screenManager->dispInsCoin();
  }
  if(lastScrnPos == 1){
    screenManager->disp1PStart();
  }
  if(lastScrnPos == 2){
    screenManager->disp2PStart();
  }
  if(lastScrnPos == 3){
    screenManager->dispNowPlaying();
  }
}