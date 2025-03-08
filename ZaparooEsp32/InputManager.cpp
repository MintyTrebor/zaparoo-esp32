#pragma once
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
    Serial.println("Do Sleep Action");
  }
  if(lastScrnPos == 1){
    Serial.println("Do Shutdown Action");
  }
  if(lastScrnPos == 2){
    //Serial.println("Do 2Player Start Action");
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
  if(lastScrnPos == 2){
    lastScrnPos = 0;
  } 
  else {
    lastScrnPos++;
  }
  if(lastScrnPos == 0){
    screenManager->dispGotoSleep();
  }
  if(lastScrnPos == 1){
    screenManager->dispPowerOff();
  }
  if(lastScrnPos == 2){
    screenManager->dispNowPlaying();
  }
  // if(lastScrnPos == 3){
    
  // }
}