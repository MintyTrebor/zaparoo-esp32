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

void InputManager:: getMainMenu(JsonDocument& menuJson){
  JsonDocument blankJson;
  blankJson["menus"][0]["menuID"] = 9999;
  //itemID 1 = Now Playing
  blankJson["menus"][0]["menuItems"][0]["itemID"] = 1;
  blankJson["menus"][0]["menuItems"][0]["itemImage"] = "dispNowPlaying";
  blankJson["menus"][0]["menuItems"][0]["itemAudio"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemText"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemTextColour"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionType"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionData"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionAudio"] = "";
  //itemID 2 = Sleep
  blankJson["menus"][0]["menuItems"][1]["itemID"] = 2;
  blankJson["menus"][0]["menuItems"][1]["itemImage"] = "dispGotoSleep";
  blankJson["menus"][0]["menuItems"][1]["itemAudio"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemText"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemTextColour"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemActionType"] = "internal";
  blankJson["menus"][0]["menuItems"][1]["itemActionData"] = "doDeepSleep";
  blankJson["menus"][0]["menuItems"][1]["itemActionAudio"] = "";
  //itemID 3 = Power Off
  blankJson["menus"][0]["menuItems"][1]["itemID"] = 3;
  blankJson["menus"][0]["menuItems"][1]["itemImage"] = "dispPowerOff";
  blankJson["menus"][0]["menuItems"][1]["itemAudio"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemText"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemTextColour"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemActionType"] = "internal";
  blankJson["menus"][0]["menuItems"][1]["itemActionData"] = "doShutdown";
  blankJson["menus"][0]["menuItems"][1]["itemActionAudio"] = "";
  menuJson = blankJson;
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
void InputManager::doRotaryTurn(int currDir){
  int currRotPos = encoder->getPosition();
  if(lastRotationPos < currRotPos){
    nextRotation();
    lastRotationPos = currRotPos;
    encoder->tick();
  }else if(lastRotationPos > currRotPos){
    previousRotation();
    lastRotationPos = currRotPos;
    encoder->tick();
  }else{
    encoder->tick();
  }
  // Serial.println("Position: " + String(currRotPos));
  // Serial.println("Direction: " + String(currDir));
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
}

void InputManager::previousRotation(){
  if(lastScrnPos == 0){
    lastScrnPos = 2;
  } 
  else {
    lastScrnPos--;
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
}

void InputManager::doDeepSleep(){
  screenManager->screenSleep();
  digitalWrite(BOARD_PN532_RF_REST, LOW); //Sleep PN532
  digitalWrite(BOARD_PWR_EN, LOW);    // Power off CC1101 and LED
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK(0), ESP_EXT1_WAKEUP_ANY_LOW);   // Hibernate using user keys
  esp_deep_sleep_start();
}
