#pragma once
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "FS.h"
#include "SD.h"

class UIDDataManager {
  private:
    bool SDCardEnabled;
    void createNewUidFile();
    void getUidFileDefaultJson(JsonDocument& fileDataJson);
    void getUidFileMenuJson(JsonDocument& fdJson);
  public:    
    UIDDataManager();
    ~UIDDataManager();
    JsonDocument currUIDJson;
    void init(bool sdEnabled);
    void createUidDataDirectory();
    void updateUidFileJson(const char* UID, JsonDocument fileDataJson);
    void getUidFileJson(const char* UID, JsonDocument& fileDataJson, bool& fileExists);
    void partialUpdUidFileJson(const char* UID, JsonDocument updateDataJson);
    void getUidFileMenuItemJson(JsonDocument& fdJson);
};