#pragma once
#include "ZaparooEsp32.hpp"
#include "InputManager.h"

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)

InputManager::InputManager() {
}

InputManager::~InputManager() {
}

void InputManager::init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan, PowerManager* pwrMan){
  screenManager = scrnMgr;
  encoder = encdr;
  feedbackManager = fbMan;
  powerManager = pwrMan;
}

void InputManager::doRotaryButton(){
  if(lastScrnPos == 0){
    Serial.println("Do Sleep Action");
    doDeepSleep();
  }
  if(lastScrnPos == 1){
    Serial.println("Do Shutdown Action");
    powerManager->doShutdown();
  }
  if(lastScrnPos == 2){
    Serial.println("Do Set Volume Action");
  }
  if(lastScrnPos == 3){
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
  if(lastScrnPos == 3){
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
    screenManager->dispSetVolume();
  }
  if(lastScrnPos == 3){
    screenManager->dispNowPlaying();
  }
}

void InputManager::doDeepSleep(){
  screenManager->screenSleep();
  digitalWrite(BOARD_PN532_RF_REST, LOW); //Sleep PN532
  digitalWrite(BOARD_PWR_EN, LOW);    // Power off CC1101 and LED
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK(0), ESP_EXT1_WAKEUP_ANY_LOW);   // Hibernate using user keys
  esp_deep_sleep_start();
}
