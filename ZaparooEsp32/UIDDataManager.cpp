#pragma once
#include "ZaparooEsp32.hpp"
#include "UIDDataManager.h"

#define UID_DATA_DIR "/zapdata"

UIDDataManager::UIDDataManager() {
}

UIDDataManager::~UIDDataManager() {
}

void UIDDataManager::init(bool sdEnabled){
  SDCardEnabled = sdEnabled;
  if(SDCardEnabled){
    if(!SD.begin(BOARD_SD_CS)) {
      Serial.println(F("failed to do SD Card"));
    }
  }
}

void UIDDataManager::getUidFileDefaultJson(JsonDocument& fileDataJson){
  fileDataJson["audio"]["launchAudio"] = "";
  fileDataJson["audio"]["removeAudio"] = "";
  fileDataJson["menus"][0]["menuID"] = 0;
  fileDataJson["menus"][0]["menuItems"][0]["itemID"] = 0;
  fileDataJson["menus"][0]["menuItems"][0]["itemImage"] = "";
  fileDataJson["menus"][0]["menuItems"][0]["itemText"] = "";
  fileDataJson["menus"][0]["menuItems"][0]["itemTextColour"] = "";
  fileDataJson["menus"][0]["menuItems"][0]["itemActionType"] = "";
  fileDataJson["menus"][0]["menuItems"][0]["itemActionData"] = "";
}


void UIDDataManager::createUidDataDirectory(){
  bool exists = true;
  if(SDCardEnabled){
    if(!SD.exists(UID_DATA_DIR)){
      SD.mkdir(UID_DATA_DIR);
    }
  }else if(!LittleFS.exists(UID_DATA_DIR)){
    LittleFS.mkdir(UID_DATA_DIR);
  }
}

void UIDDataManager::updateUidFileJson(const char* UID, JsonDocument fileDataJson){
  File uidFile;
  String tmpJson = "";
  String filePath = String(UID_DATA_DIR) + "/" + String(UID) + ".json";
  if(SDCardEnabled){
    uidFile = SD.open(filePath, FILE_WRITE);
  }else{
    uidFile = LittleFS.open(filePath, FILE_WRITE);
  }
  serializeJson(fileDataJson, tmpJson);
  uidFile.print(tmpJson);
  uidFile.close();
}

void UIDDataManager::getUidFileJson(const char* UID, JsonDocument& fileDataJson){
  String filePath = String(UID_DATA_DIR) + "/" + String(UID) + ".json";
  File uidFile;
  bool exists = true;
  if(SDCardEnabled){
    if(SD.exists(filePath)){
      uidFile = SD.open(filePath);
      exists = false;
    }
  }else if(!LittleFS.exists(filePath)){
    uidFile = LittleFS.open(filePath, FILE_WRITE);
    exists = false;
  }
  if(!exists){
    //return empty data set
    getUidFileDefaultJson(fileDataJson);
    return;
  }else {
    while (uidFile.available()) { 
      DeserializationError error = deserializeJson(fileDataJson, uidFile);
      if(!error){
        uidFile.close();
        return;
      }
    }
    uidFile.close();
    }
}