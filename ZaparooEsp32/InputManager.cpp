#pragma once
#include "ZaparooEsp32.hpp"
#include "InputManager.h"

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)


InputManager::InputManager() {
}

InputManager::~InputManager() {
}

void InputManager::init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan, PowerManager* pwrMan, UIDDataManager* UIDDMan){
  screenManager = scrnMgr;
  encoder = encdr;
  feedbackManager = fbMan;
  powerManager = pwrMan;
  uidDataMan = UIDDMan;
}

void InputManager:: getMainMenu(JsonDocument& menuJson){
  JsonDocument blankJson;
  blankJson["menus"][0]["menuID"] = "9999";
  //itemID 1 = Now Playing
  blankJson["menus"][0]["menuItems"][0]["itemID"] = "1";
  blankJson["menus"][0]["menuItems"][0]["itemImage"] = "dispNowPlaying";
  blankJson["menus"][0]["menuItems"][0]["itemAudio"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemText"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemTextColour"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionType"] = "menu";
  blankJson["menus"][0]["menuItems"][0]["itemActionData"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionAudio"] = "";
  //itemID 2 = Sleep
  blankJson["menus"][0]["menuItems"][1]["itemID"] = "2";
  blankJson["menus"][0]["menuItems"][1]["itemImage"] = "dispGotoSleep";
  blankJson["menus"][0]["menuItems"][1]["itemAudio"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemText"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemTextColour"] = "";
  blankJson["menus"][0]["menuItems"][1]["itemActionType"] = "internal";
  blankJson["menus"][0]["menuItems"][1]["itemActionData"] = "doDeepSleep";
  blankJson["menus"][0]["menuItems"][1]["itemActionAudio"] = "";
  //itemID 3 = Power Off
  blankJson["menus"][0]["menuItems"][2]["itemID"] = "3";
  blankJson["menus"][0]["menuItems"][2]["itemImage"] = "dispPowerOff";
  blankJson["menus"][0]["menuItems"][2]["itemAudio"] = "";
  blankJson["menus"][0]["menuItems"][2]["itemText"] = "";
  blankJson["menus"][0]["menuItems"][2]["itemTextColour"] = "";
  blankJson["menus"][0]["menuItems"][2]["itemActionType"] = "internal";
  blankJson["menus"][0]["menuItems"][2]["itemActionData"] = "doShutdown";
  blankJson["menus"][0]["menuItems"][2]["itemActionAudio"] = "";
  menuJson = blankJson;
}

void InputManager::setCurrMenu(String menuID){
  Serial.println("SettingCurrMenu To: " + String(menuID));
  JsonDocument tmpJson;
  if(menuID == 0){
    currMenuItemID = "9999";
  }else {
    currMenuItemID = menuID;
  }
  if(currMenuItemID == "9999"){
    getMainMenu(tmpJson);
    currMenuJson = tmpJson["menus"][0];
    currMenuItemCount = currMenuJson["menuItems"].size();
  }else {    
    getMenu(menuID, tmpJson);
    currMenuJson = tmpJson;
    //add the exit menu into the menuitems array
    JsonDocument exitMenuItem;
    uidDataMan->getUidFileMenuItemJson(exitMenuItem);
    exitMenuItem["itemID"] = "exitMenu";
    exitMenuItem["itemImage"] = currMenuJson["exitMenuImg"];
    exitMenuItem["itemText"] = currMenuJson["exitMenuText"];
    exitMenuItem["itemTextColour"] = currMenuJson["exitMenuTextColour"];
    exitMenuItem["itemActionType"] = "menu";
    exitMenuItem["itemActionData"] = currMenuJson["exitMenuID"];
    exitMenuItem["itemActionAudio"] = currMenuJson["exitMenuActionAudio"];
    currMenuJson["menuItems"].add(exitMenuItem);
    currMenuItemCount = currMenuJson["menuItems"].size();
  }
  
  
  if(currMenuItemCount > 0){currMenuItemCount--;}
  String tmpStr = "";
  serializeJson(tmpJson, tmpStr);
  Serial.println("tmpJson: " + tmpStr);
  tmpStr = "";
  serializeJson(currMenuJson, tmpStr);
  Serial.println("currMenuJson: " + tmpStr);
  Serial.println("currMenuItemCount: " + String(currMenuItemCount));
  currMenuItemPos = 0;
}

void InputManager::setupMenu(){
  JsonDocument tmpJson;
  tmpJson = uidDataMan->currUIDJson;
  //first set the default sub menu id 
  if(tmpJson["launchImgMenuID"].as<String>().length() > 0){
    defSubMenuID = tmpJson["launchImgMenuID"].as<String>();
    Serial.println("Setting Def Sub Menu ID to: " + defSubMenuID);
    currMenuItemPos == 0;
    doCurrMenuItem();
  }
  
};

void InputManager::getMenu(String menuID, JsonDocument& menuJson){
  Serial.println("GetMenu");
  JsonDocument tmpJson;
  tmpJson = uidDataMan->currUIDJson;
  if(tmpJson["menus"].is<JsonArray>() && !tmpJson["menus"].isNull()){ 
    for (JsonObject menu : tmpJson["menus"].as<JsonArray>()) {
        if (menu["menuID"] == menuID) {
            Serial.println("Found Menu");
            menuJson = menu;
            String tmpStr = "";
            serializeJson(menu, tmpStr);
            Serial.println("getmenu() menu json: " + tmpStr);
            return;
        }
    }
  }else{
    //should not happen but just in case!
    getMainMenu(tmpJson);
    menuJson = tmpJson;
  }
}

void InputManager::doRotaryButton(){
  Serial.println("doRotaryButton");
  String tmpActionType = currMenuItemJson["itemActionType"].as<String>();
  String tmpActionData = currMenuItemJson["itemActionData"].as<String>();
  String tmpMenuID = currMenuItemJson["itemID"].as<String>();
  const char* tmpActionAudio = currMenuItemJson["itemActionAudio"].as<String>().c_str();

  //do default menu items check
  if(tmpMenuID == "1" && defSubMenuID.length() > 0){
    Serial.println("Opening default sub menu");
    setCurrMenu(defSubMenuID);
    currMenuItemPos = 0;
    doCurrMenuItem();
  }else if(tmpActionType == "internal"){
    Serial.println("Do Internal");
    if(tmpActionData == "doDeepSleep"){
      doDeepSleep();
    }
    if(tmpActionData == "doShutdown"){
      powerManager->doShutdown();
    }
  }else if(tmpActionType == "menu" && tmpActionData.length() > 0){
    setCurrMenu(tmpActionData);
    currMenuItemPos = 0;
    doCurrMenuItem();
  }else if(tmpActionType == "launchGame" && tmpActionData.length() > 0){
    Serial.println("Launch Game From Menu Click");
  }else if(tmpActionType == "launchScript" && tmpActionData.length() > 0){
    Serial.println("Launch Script From Menu Click");
  }else {
    Serial.println("Failed to find action");
  }

}

void InputManager::doRotaryTurn(int currDir){
  int currRotPos = encoder->getPosition();
  if(lastRotationPos < currRotPos){
    Serial.println("Rotate Next");
    if(currMenuItemPos == currMenuItemCount){
      currMenuItemPos = 0;
    }else {
      currMenuItemPos++;
    }
    lastRotationPos = currRotPos;
    encoder->tick();
  }else if(lastRotationPos > currRotPos){
    Serial.println("Rotate Prev");
    if(currMenuItemPos == 0){
      currMenuItemPos = currMenuItemCount;
    }else {
      currMenuItemPos--;
    }    
    lastRotationPos = currRotPos;
    encoder->tick();
  }else{
    encoder->tick();
  }
  Serial.println("currMenuItemPos: " + String(currMenuItemPos));
  doCurrMenuItem();
}

void InputManager::doCurrMenuItem(){
  Serial.println("doCurrMenuItem");
  currMenuItemJson = currMenuJson["menuItems"][currMenuItemPos];
  String tmpStr = "";
  serializeJson(currMenuItemJson, tmpStr);
  Serial.println("currMenuItemJson: " + tmpStr);
  String tmpImgPath = currMenuItemJson["itemImage"].as<String>();
  //do default menu items check
  if(tmpImgPath == "dispNowPlaying"){
    Serial.println("show NowPlaying");
    screenManager->dispNowPlaying();
  }else if(tmpImgPath == "dispPowerOff"){
    Serial.println("show poweroff");
    screenManager->dispPowerOff();
  }else if(tmpImgPath == "dispGotoSleep"){
    Serial.println("show pwr off");
    screenManager->dispGotoSleep();
  }else {
    Serial.println("show other");
    screenManager->dispJpgImg(tmpImgPath.c_str());
  }
}


void InputManager::doDeepSleep(){
  screenManager->screenSleep();
  digitalWrite(BOARD_PN532_RF_REST, LOW); //Sleep PN532
  digitalWrite(BOARD_PWR_EN, LOW);    // Power off CC1101 and LED
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK(0), ESP_EXT1_WAKEUP_ANY_LOW);   // Hibernate using user keys
  esp_deep_sleep_start();
}
