#pragma once
#include "ZaparooEsp32.hpp"
#include "InputManager.h"
//#include "mainMenu.h"

//String ZAP_URL = "ws://<replace>:7497" + String(ZaparooLaunchApi::wsPath);

#define BUTTON_PIN_BITMASK(GPIO) (1ULL << GPIO)


InputManager::InputManager() {
}

InputManager::~InputManager() {
}

void InputManager::init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan, PowerManager* pwrMan, UIDDataManager* UIDDMan, ZaparooLaunchApi* ZapCli, bool isSerial, String stmIP, String zpIP){
  screenManager = scrnMgr;
  encoder = encdr;
  feedbackManager = fbMan;
  powerManager = pwrMan;
  uidDataMan = UIDDMan;
  ZapLaunchClient = ZapCli;
  isSerialOnly = isSerial;
  IPSteam = stmIP;
  IPZap = zpIP;
}

void InputManager:: getMainMenu(JsonDocument& menuJson){
  JsonDocument blankJson;
  const char *mainMenuJson = 
  #include "mainMenu.h"
  ;
  String tmpJsonStr = String(mainMenuJson);
  //Serial.println("Main Menu JSON : " + tmpJsonStr);
  DeserializationError error = deserializeJson(blankJson, tmpJsonStr);
  if(!error){
    menuJson = blankJson;
    return;
  }else{
    Serial.println("Bad Menu JSON");
  }
}

void InputManager::setCurrMenu(String menuID){
  Serial.println("SettingCurrMenu To: ");
  Serial.println(menuID);
  JsonDocument tmpJson;
  currMenuJson = {};
  if(menuID == 0){
    currMenuItemID = "9999";
  }else {
    currMenuItemID = menuID;
  }
  if(currMenuItemID == "9999"){
    getMainMenu(currMenuJson);
    currMenuItemCount = currMenuJson["args"]["items"].size();
    Serial.println("currMenuItemCount: " + String(currMenuItemCount));
  }else {    
    getMenu(menuID, currMenuJson);
    currMenuItemCount = currMenuJson["args"]["items"].size();
  }  
  if(currMenuItemCount > 0){currMenuItemCount--;}
  currMenuItemPos = 0;
}

void InputManager::resetToDefaultMenu(){
  screenManager->setNowPlayingPath("");
  setCurrMenu("9999");
}

void InputManager::setupMenu(){
  JsonDocument tmpJson;
  tmpJson = uidDataMan->currUIDJson;
  //set the default sub menu id 
  if(currMenuItemID == "9999" && tmpJson["cmds"][0]["cmd"].as<String>() == "evaluate"){
    if(tmpJson["cmds"][0]["args"]["client"][0]["type"].as<String>() == "reader"){
      if(tmpJson["cmds"][0]["args"]["client"][0]["args"]["input"]["buttons"][0]["args"]["actions"][0]["cmd"].as<String>() == "ui.picker"){
        defSubMenuID = tmpJson["cmds"][0]["args"]["client"][0]["args"]["input"]["buttons"][0]["args"]["actions"][0]["args"]["uiPickerID"].as<String>();
      }
    }
    currMenuItemPos == 0;
    showCurrMenuItem();
  }else {
    defSubMenuID = "";
    currMenuItemPos == 0;
    showCurrMenuItem();
  }
  
};

void InputManager::getMenu(String menuID, JsonDocument& menuJson){
  //Serial.println("GetMenu: " + String(menuID));
  JsonDocument tmpJson;
  tmpJson = uidDataMan->currUIDJson;
  if(tmpJson["cmds"].is<JsonArray>() && !tmpJson["cmds"].isNull()){ 
    for (JsonObject menu : tmpJson["cmds"].as<JsonArray>()) {
        if (menu["id"] == menuID) {
            //Serial.println("Found Menu");
            menuJson = menu;
            return;
        }
        if (menu["cmd"] == "ui.picker"){
          if(menu["args"]["pickers"].is<JsonArray>() && !menu["args"]["pickers"].isNull()){
            for (JsonObject picker : menu["args"]["pickers"].as<JsonArray>()){
              if (picker["id"] == menuID) {
                  //Serial.println("Found Menu");
                  menuJson = picker;
                  return;
              }
            }
          }
        }
    }
  }else{
    //should not happen but just in case!
    getMainMenu(tmpJson);
    menuJson = tmpJson;
  }
}

void InputManager::doRotaryButton(){
  String tmpActionType = currMenuItemJson["cmd"].as<String>();
  String tmpActionData = currMenuItemJson["args"]["zapscript"].as<String>();
  String tmpMenuID = currMenuItemJson["id"].as<String>();
  //do default menu items check
  if(tmpMenuID == "9999-1" && defSubMenuID.length() > 0){
    setCurrMenu(defSubMenuID);
    currMenuItemPos = 0;
    showCurrMenuItem();
  }else if(tmpMenuID == "9999-2"){
    doDeepSleep();
  }else if(tmpMenuID == "9999-3"){
    powerManager->doShutdown();
  }else if(tmpActionType == "evaluate" && tmpActionData.length() > 0){
    sendToZap(tmpActionData);
    if(currMenuItemJson["args"]["client"][0]["args"]["audio"]["onClickAudioPath"].as<String>().length() > 0){
      String clkAudio = currMenuItemJson["args"]["client"][0]["args"]["audio"]["onClickAudioPath"];
      doInputEventAudio(clkAudio.c_str());
    }
    JsonDocument tmpJson = currMenuItemJson["args"]["client"][0]["args"]["input"]["buttons"][0]["args"];
    if(tmpJson["actions"].is<JsonArray>() && !tmpJson["actions"].isNull()){ 
      for (JsonObject action : tmpJson["actions"].as<JsonArray>()) {
        String uiPickID = action["args"]["uiPickerID"];
        String actCmd = action["cmd"];
        String actAudio = action["args"]["audio"]["onClickAudioPath"];
        String actLaunchAudio = action["args"]["audio"]["launchAudioPath"];
        String actZapScr = action["args"]["zapscript"];
        if(uiPickID.length() > 0 && actCmd == "ui.picker"){
          //Serial.println("picker: "  + uiPickID);
          setCurrMenu(uiPickID);
          currMenuItemPos = 0;
          showCurrMenuItem();
          if(actAudio.length() > 0){
            const char* tmpActionAudio = actAudio.c_str();
            doInputEventAudio(tmpActionAudio);
          }
        }else if(actCmd == "evaluate" && actZapScr.length() > 0){
          //Serial.println("zapcmd: "  + actZapScr);
          sendToZap(actZapScr);
          if(actLaunchAudio.length() > 0){
            const char* tmpActionAudio = actLaunchAudio.c_str();
            doInputEventAudio(tmpActionAudio);
          }
        }
      }
    }
  }else if((tmpActionType == "evaluate" && tmpActionData.length() == 0) || (tmpActionType == "evaluate.client" && tmpActionData.length() == 0)){
    //Serial.println("Do Action From Menu Click: "  + tmpActionData);
    JsonDocument tmpJson = currMenuItemJson["args"]["client"][0]["args"]["input"]["buttons"][0]["args"];
    if(tmpJson["actions"].is<JsonArray>() && !tmpJson["actions"].isNull()){ 
      for (JsonObject action : tmpJson["actions"].as<JsonArray>()) {
        String uiPickID = action["args"]["uiPickerID"];
        String actCmd = action["cmd"];
        String actAudio = action["args"]["audio"]["onClickAudioPath"];
        String actLaunchAudio = action["args"]["audio"]["launchAudioPath"];
        String actZapScr = action["args"]["zapscript"];
        if(uiPickID.length() > 0 && actCmd == "ui.picker"){
          //Serial.println("picker: "  + uiPickID);
          setCurrMenu(uiPickID);
          currMenuItemPos = 0;
          showCurrMenuItem();
          if(actAudio.length() > 0){
            const char* tmpActionAudio = actAudio.c_str();
            doInputEventAudio(tmpActionAudio);
          }
        }else if(actCmd == "evaluate" && actZapScr.length() > 0){
          //Serial.println("zapcmd: "  + actZapScr);
          sendToZap(actZapScr);
          if(actLaunchAudio.length() > 0){
            const char* tmpActionAudio = actLaunchAudio.c_str();
            doInputEventAudio(tmpActionAudio);
          }
        }
      }
    }
  }else {
    Serial.println("Failed to find action");
  }

}

void InputManager::doInputEventAudio(const char* aPath){
  if(strlen(aPath) == 0){
    return;
  }
  //Serial.println("Inpt Aud Path: " + String(aPath));
  feedbackManager->playAudio(aPath);
}

void InputManager::doRotaryTurn(int currDir){
  int currRotPos = encoder->getPosition();
  if(lastRotationPos > currRotPos){
    //Serial.println("Rotate Next");
    if(currMenuItemPos == currMenuItemCount){
      currMenuItemPos = 0;
    }else {
      currMenuItemPos++;
    }
    lastRotationPos = currRotPos;
    encoder->tick();
  }else if(lastRotationPos < currRotPos){
    //Serial.println("Rotate Prev");
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
  showCurrMenuItem();
}

void InputManager::showCurrMenuItem(){
  currMenuItemJson = currMenuJson["args"]["items"][currMenuItemPos];
  String tmpImgPath = currMenuItemJson["args"]["client"][0]["args"]["display"]["imgPath"].as<String>();
  //do default menu items check
  if(tmpImgPath == "dispNowPlaying"){
    screenManager->dispNowPlaying();
  }else if(tmpImgPath == "dispPowerOff"){
    screenManager->dispPowerOff();
  }else if(tmpImgPath == "dispGotoSleep"){
    screenManager->dispGotoSleep();
  }else {
    String tmpTxtStr = currMenuItemJson["args"]["client"][0]["args"]["display"]["displayText"].as<String>();
    JsonDocument tmpTxtRGB;
    tmpTxtRGB["r"] = currMenuItemJson["args"]["client"][0]["args"]["display"]["textColour"]["r"];
    tmpTxtRGB["g"] = currMenuItemJson["args"]["client"][0]["args"]["display"]["textColour"]["g"];
    tmpTxtRGB["b"] = currMenuItemJson["args"]["client"][0]["args"]["display"]["textColour"]["b"];
    JsonDocument tmpScrnRBG;
    tmpScrnRBG["r"] = currMenuItemJson["args"]["client"][0]["args"]["display"]["screenColour"]["r"];
    tmpScrnRBG["g"] = currMenuItemJson["args"]["client"][0]["args"]["display"]["screenColour"]["g"];
    tmpScrnRBG["b"] = currMenuItemJson["args"]["client"][0]["args"]["display"]["screenColour"]["b"];
    const char* tmpTextChar = tmpTxtStr.c_str();
    if (strlen(tmpTextChar) > 0) {
      screenManager->drawTextStr(tmpTxtStr, tmpTxtRGB, tmpScrnRBG, currMenuItemJson["args"]["client"][0]["args"]["display"]["fontSize"].as<int>(), currMenuItemJson["args"]["client"][0]["args"]["display"]["fontNumber"].as<int>());
    }else{
      screenManager->dispJpgImg(tmpImgPath.c_str());
    }
  }
}

void InputManager::doDeepSleep(){
  feedbackManager->ledRingSleep();
  screenManager->screenSleep();
  digitalWrite(BOARD_PN532_RF_REST, LOW); //Sleep PN532
  digitalWrite(BOARD_PWR_EN, LOW);    // Power off CC1101 and LED
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK(0), ESP_EXT1_WAKEUP_ANY_LOW);   // Hibernate using user keys
  esp_deep_sleep_start();
}

bool InputManager::sendToZap(String& gamePath) {
  String ZAPURL = "ws://<replace>:7497" + String(ZaparooLaunchApi::wsPath);
  String lastSerialCmd = "";
  bool sent = false;
  if (isSerialOnly) {
    lastSerialCmd = "SCAN\ttext=" + gamePath;
    if(!feedbackManager->resetOnRemove){
      lastSerialCmd = lastSerialCmd + "\tremovable=no";
      Serial.println(lastSerialCmd);
    }
    Serial.flush();
    sent = true;
  } else {
    String newURL = ZAPURL;
    newURL.replace("<replace>", gamePath.startsWith("steam://") ? IPSteam : IPZap);
    ZapLaunchClient->url(newURL);
    int code = ZapLaunchClient->launch(gamePath);
    Serial.println("resp code: " + String(code));
    if (code > 0) {
      feedbackManager->expressError(code);
    } else {
      sent = true;
    }
  }
  return sent;
}
