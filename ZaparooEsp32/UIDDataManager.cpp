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

void UIDDataManager::getUidFileDefaultJson(JsonDocument& fdJson){
  JsonDocument blankJson;
  blankJson["launchAudio"] = "";
  blankJson["removeAudio"] = "";
  blankJson["launchImg"] = "";
  blankJson["launchImgMenuID"] = 0;
  blankJson["menus"][0]["menuID"] = 0;
  blankJson["menus"][0]["menuItems"][0]["itemID"] = 0;
  blankJson["menus"][0]["menuItems"][0]["itemImage"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemAudio"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemText"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemTextColour"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionType"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionData"] = "";
  blankJson["menus"][0]["menuItems"][0]["itemActionAudio"] = "";
  fdJson = blankJson;
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

void UIDDataManager::partialUpdUidFileJson(const char* UID, JsonDocument updateDataJson){
  File uidFile;
  String tmpJson = "";
  String filePath = String(UID_DATA_DIR) + "/" + String(UID) + ".json";
  bool exists = true;
  JsonDocument blankJson;
  if(SDCardEnabled){
    if(SD.exists(filePath.c_str())){
      uidFile = SD.open(filePath.c_str());
    } else {
      exists = false;
    }
  }else if(LittleFS.exists(filePath.c_str())){
    uidFile = LittleFS.open(filePath.c_str());
  }else {
    exists = false;
  }
  if(!exists){
    Serial.println("Did not find file");
    //get empty data set    
    getUidFileDefaultJson(blankJson);    
  }else {
    Serial.println("Found file");
    while (uidFile.available()) { 
      DeserializationError error = deserializeJson(blankJson, uidFile);        
    }
    uidFile.close();
  }
  if(SDCardEnabled){
    uidFile = SD.open(filePath, FILE_WRITE);
  }else{
    uidFile = LittleFS.open(filePath, FILE_WRITE);
  }
  blankJson["launchAudio"] = updateDataJson["launchAudio"];
  blankJson["removeAudio"] = updateDataJson["removeAudio"];
  blankJson["launchImg"] = updateDataJson["launchImg"];
  blankJson["launchImgMenuID"] = updateDataJson["launchImgMenuID"];
  serializeJson(blankJson, tmpJson);
  uidFile.print(tmpJson);
  uidFile.close();
}

void UIDDataManager::updateUidFileJson(const char* UID, JsonDocument updateDataJson){
  File uidFile;
  String tmpJson = "";
  String filePath = String(UID_DATA_DIR) + "/" + String(UID) + ".json";
  if(SDCardEnabled){
    uidFile = SD.open(filePath, FILE_WRITE);
  }else{
    uidFile = LittleFS.open(filePath, FILE_WRITE);
  }
  serializeJson(updateDataJson, tmpJson);
  uidFile.print(tmpJson);
  uidFile.close();
}

void UIDDataManager::getUidFileJson(const char* UID, JsonDocument& loadedDataJson, bool& fileExists){
  String filePath = String(UID_DATA_DIR) + "/" + String(UID) + ".json";
  Serial.println("Looking for file: " + filePath);
  File uidFile;
  bool exists = true;
  if(SDCardEnabled){
    if(SD.exists(filePath.c_str())){
      uidFile = SD.open(filePath.c_str());
      fileExists = true;
    } else {
      exists = false;
      fileExists = false;
    }
  }else if(LittleFS.exists(filePath.c_str())){
    uidFile = LittleFS.open(filePath.c_str());
    fileExists = true;
  }else {
    exists = false;
    fileExists = false;
  }
  if(!exists){
    Serial.println("Did not find file");
    //return empty data set
    JsonDocument blankJson;
    getUidFileDefaultJson(blankJson);
    loadedDataJson = blankJson;
    return;
  }else {
    Serial.println("Found file");
    while (uidFile.available()) { 
      DeserializationError error = deserializeJson(loadedDataJson, uidFile);
      if(!error){
        uidFile.close();
        return;
      }
    }
    uidFile.close();
  }
}