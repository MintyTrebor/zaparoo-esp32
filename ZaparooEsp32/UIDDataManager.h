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
    void createNewUidFile(const char* UID, JsonDocument& fileDataJson);
    
  public:    
    UIDDataManager();
    ~UIDDataManager();
    void init(bool sdEnabled);
    void createUidDataDirectory();
    void updateUidFileJson(const char* UID, JsonDocument& fileDataJson);
    void getUidFileJson(const char* UID, JsonDocument& fileDataJson);
    
};