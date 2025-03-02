#include "ZaparooEsp32.hpp"
#include "InputManager.h"

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

//Prefences has a 14 character limit for key
void InputManager::init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan){
  screenManager = scrnMgr;
  encoder = encdr;
  feedbackManager = fbMan;
}

void InputManager::doRotaryButton(){
  //do button action here
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