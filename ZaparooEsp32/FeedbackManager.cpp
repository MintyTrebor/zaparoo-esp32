#include "FeedbackManager.h"
#include "ZaparooEsp32.hpp"

#define UID_MAP_FILE "/uidExtdRecord.json"

FeedbackManager::FeedbackManager() {
    out = nullptr;
}

FeedbackManager::~FeedbackManager() {
    if (out) {
        delete out;
    }
}

//Prefences has a 14 character limit for key
void FeedbackManager::init(Preferences* prefs) {
    preferences = prefs;
    audioGain = preferences->getFloat("audioGain", 1.0);
    wifiLedEnabled = preferences->getBool("wifiLed", false);
    wifiLedEnabled = false;
    motorEnabled = preferences->getBool("motor", false);
    launchLedEnabled = preferences->getBool("launchLed", false);
    audioEnabled = preferences->getBool("audio", false);
    pwrLedEnabled = preferences->getBool("pwrLed", false);
    resetOnRemove = preferences->getBool("resetOnRemove", true);
    sdCardEnabled = preferences->getBool("sdCard", false);
    buzzOnDetect = preferences->getBool("buzzOnDetect", true);
    buzzOnLaunch = preferences->getBool("buzzOnLaunch", true);
    buzzOnRemove = preferences->getBool("buzzOnRemove", true);
    buzzOnError = preferences->getBool("buzzOnError", true);
    defaultInsertAudio = preferences->getString("insertAudio", "");
    defaultLaunchAudio = preferences->getString("launchAudio", "");
    defaultRemoveAudio = preferences->getString("removeAudio", "");
    defaultErrorAudio = preferences->getString("errorAudio", "");

    motorPin = preferences->getInt("motorPin", 32);
    launchLedPin = preferences->getInt("launchLedPin", 33);
    wifiLedPin = preferences->getInt("wifiLedPin", 2);
    pwrLedPin = preferences->getInt("pwrLedPin", 15);

    // Read the I2S pins from preferences but don't save them in member variables
    i2sBclkPin = preferences->getInt("i2sBclkPin", 27);
    i2sLrcPin = preferences->getInt("i2sLrcPin", 26);
    i2sDoutPin = preferences->getInt("i2sDoutPin", 25);
    createUidMappingFile();
    setupPins();

}

void FeedbackManager::createUidMappingFile(){
  File uidFile;
  bool exists = true;
  if (sdCardEnabled){
    if(!SD.exists(UID_MAP_FILE)){
      uidFile = SD.open(UID_MAP_FILE, FILE_WRITE);
      exists = false;
    }
  }else if(!LittleFS.exists(UID_MAP_FILE)){
    uidFile = LittleFS.open(UID_MAP_FILE, FILE_WRITE);
    exists = false;
  }
  if(!exists){
    JsonDocument tmpDoc;
    tmpDoc["UID_ExtdRecs"][0]["UID"] = "";
    tmpDoc["UID_ExtdRecs"][0]["launchAudio"] = "";
    tmpDoc["UID_ExtdRecs"][0]["removeAudio"] = "";
    String tmpJson = "";
    serializeJson(tmpDoc, tmpJson);
    uidFile.print(tmpJson);
    uidFile.close();
  }
}


void FeedbackManager::setupPins() {
  if (motorEnabled) {
    pinMode(motorPin, OUTPUT);
  }
  if (wifiLedEnabled) {
    pinMode(wifiLedPin, OUTPUT);
  }
  if (launchLedEnabled) {
    pinMode(launchLedPin, OUTPUT);
  }
  if (pwrLedEnabled) {
    pinMode(pwrLedPin, OUTPUT);
    digitalWrite(pwrLedPin, HIGH);
  }
  if (audioEnabled) {
    delete out;
    out = NULL;
    out = new AudioOutputI2S();
    out->SetPinout(i2sBclkPin, i2sLrcPin, i2sDoutPin);
    out->SetChannels(1);
    out->SetGain(audioGain);
    if (sdCardEnabled) {
      if (!SD.begin(SS_PIN)) {
        Serial.println(F("failed to do SD Card"));
      }
    }
  }
}

void FeedbackManager::update(JsonDocument& doc) {
    serializeJson(doc, Serial);
    Serial.println();
    if (doc["data"].containsKey("audioGain")) {
        audioGain = doc["data"]["audioGain"].as<float>();
        preferences->putFloat("audioGain", audioGain);
    }
    if (doc["data"].containsKey("wifiLedEnabled")) {
        wifiLedEnabled = doc["data"]["wifiLedEnabled"].as<bool>();
        preferences->putBool("wifiLed", wifiLedEnabled);
    }
    if (doc["data"].containsKey("motorEnabled")) {
        motorEnabled = doc["data"]["motorEnabled"].as<bool>();
        preferences->putBool("motor", motorEnabled);
    }
    if (doc["data"].containsKey("launchLedEnabled")) {
        launchLedEnabled = doc["data"]["launchLedEnabled"].as<bool>();
        preferences->putBool("launchLed", launchLedEnabled);
    }
    if (doc["data"].containsKey("audioEnabled")) {
        audioEnabled = doc["data"]["audioEnabled"].as<bool>();
        preferences->putBool("audio", audioEnabled);
    }
    if (doc["data"].containsKey("pwrLedEnabled")) {
        pwrLedEnabled = doc["data"]["pwrLedEnabled"].as<bool>();
        preferences->putBool("pwrLed", pwrLedEnabled);
    }
    if (doc["data"].containsKey("resetOnRemove")) {
        resetOnRemove = doc["data"]["resetOnRemove"].as<bool>();
        preferences->putBool("resetOnRemove", resetOnRemove);
    }
    if (doc["data"].containsKey("sdCardEnabled")) {
        sdCardEnabled = doc["data"]["sdCardEnabled"].as<bool>();
        preferences->putBool("sdCard", sdCardEnabled);
    }
    if (doc["data"].containsKey("buzzOnDetect")) {
        buzzOnDetect = doc["data"]["buzzOnDetect"].as<bool>();
        preferences->putBool("buzzOnDetect", buzzOnDetect);
    }
    if (doc["data"].containsKey("buzzOnLaunch")) {
        buzzOnLaunch = doc["data"]["buzzOnLaunch"].as<bool>();
        preferences->putBool("buzzOnLaunch", buzzOnLaunch);
    }
    if (doc["data"].containsKey("buzzOnRemove")) {
        buzzOnRemove = doc["data"]["buzzOnRemove"].as<bool>();
        preferences->putBool("buzzOnRemove", buzzOnRemove);
    }
    if (doc["data"].containsKey("buzzOnError")) {
        buzzOnError = doc["data"]["buzzOnError"].as<bool>();
        preferences->putBool("buzzOnError", buzzOnError);
    }
    if (doc["data"].containsKey("defaultInsertAudio")) {
        defaultInsertAudio = doc["data"]["defaultInsertAudio"].as<String>();
        preferences->putString("insertAudio", defaultInsertAudio);
    }
    if (doc["data"].containsKey("defaultLaunchAudio")) {
        defaultLaunchAudio = doc["data"]["defaultLaunchAudio"].as<String>();
        preferences->putString("launchAudio", defaultLaunchAudio);
        Serial.println(preferences->getString("launchAudio", "Not Found"));
    }
    if (doc["data"].containsKey("defaultRemoveAudio")) {
        defaultRemoveAudio = doc["data"]["defaultRemoveAudio"].as<String>();
        preferences->putString("removeAudio", defaultRemoveAudio);
    }
    if (doc["data"].containsKey("defaultErrorAudio")) {
        defaultErrorAudio = doc["data"]["defaultErrorAudio"].as<String>();
        preferences->putString("errorAudio", defaultErrorAudio);
    }

    // Pin assignments - saved to Preferences
    if (doc["data"].containsKey("motorPin")) {
        motorPin = doc["data"]["motorPin"].as<int>();
        preferences->putInt("motorPin", motorPin);
    }
    if (doc["data"].containsKey("launchLedPin")) {
        launchLedPin = doc["data"]["launchLedPin"].as<int>();
        preferences->putInt("launchLedPin", launchLedPin);
    }
    if (doc["data"].containsKey("wifiLedPin")) {
        wifiLedPin = doc["data"]["wifiLedPin"].as<int>();
        preferences->putInt("wifiLedPin", wifiLedPin);
    }
    if (doc["data"].containsKey("pwrLedPin")) {
        pwrLedPin = doc["data"]["pwrLedPin"].as<int>();
        preferences->putInt("pwrLedPin", pwrLedPin);
    }
    if (doc["data"].containsKey("i2sBclkPin")) {
        i2sBclkPin = doc["data"]["i2sBclkPin"].as<int>();
        preferences->putInt("i2sBclkPin", i2sBclkPin);
    }
    if (doc["data"].containsKey("i2sLrcPin")) {
        i2sLrcPin = doc["data"]["i2sLrcPin"].as<int>();
        preferences->putInt("i2sLrcPin", i2sLrcPin);
    }
    if (doc["data"].containsKey("i2sDoutPin")) {
        i2sDoutPin = doc["data"]["i2sDoutPin"].as<int>();
        preferences->putInt("i2sDoutPin", i2sDoutPin);
    }
}


void FeedbackManager::set(JsonDocument& doc) {
    doc["data"]["audioGain"] = audioGain;
    doc["data"]["wifiLedEnabled"] = wifiLedEnabled;
    doc["data"]["motorEnabled"] = motorEnabled;
    doc["data"]["launchLedEnabled"] = launchLedEnabled;
    doc["data"]["audioEnabled"] = audioEnabled;
    doc["data"]["pwrLedEnabled"] = pwrLedEnabled;
    doc["data"]["resetOnRemove"] = resetOnRemove;
    doc["data"]["sdCardEnabled"] = sdCardEnabled;
    doc["data"]["buzzOnDetect"] = buzzOnDetect;
    doc["data"]["buzzOnLaunch"] = buzzOnLaunch;
    doc["data"]["buzzOnRemove"] = buzzOnRemove;
    doc["data"]["buzzOnError"] = buzzOnError;
    doc["data"]["defaultInsertAudio"] = defaultInsertAudio;
    doc["data"]["defaultLaunchAudio"] = defaultLaunchAudio;
    doc["data"]["defaultRemoveAudio"] = defaultRemoveAudio;
    doc["data"]["defaultErrorAudio"] = defaultErrorAudio;
    doc["data"]["motorPin"] = motorPin;
    doc["data"]["launchLedPin"] = launchLedPin;
    doc["data"]["wifiLedPin"] = wifiLedPin;
    doc["data"]["pwrLedPin"] = pwrLedPin;
    doc["data"]["i2sBclkPin"] = i2sBclkPin;
    doc["data"]["i2sLrcPin"] = i2sLrcPin;
    doc["data"]["i2sDoutPin"] = i2sDoutPin;
}


void FeedbackManager::motorOn(int predelay) {
    if (motorEnabled) {
        delay(predelay);
        analogWrite(motorPin, 255);
    }
}

void FeedbackManager::motorOff(int predelay) {
    if (motorEnabled) {
        delay(predelay);
        analogWrite(motorPin, 0);
    }
}

void FeedbackManager::launchLedOn(int predelay) {
    if (launchLedEnabled) {
        delay(predelay);
        digitalWrite(launchLedPin, HIGH);
    }
}

void FeedbackManager::launchLedOff(int predelay, int postDelay) {
    if (launchLedEnabled) {
        delay(predelay);
        digitalWrite(launchLedPin, LOW);
        delay(postDelay);
    }
}

void FeedbackManager::wifiLedOn() {
    if (wifiLedEnabled) {
        digitalWrite(wifiLedPin, HIGH);
    }
}

void FeedbackManager::wifiLedOff() {
    if (wifiLedEnabled) {
        digitalWrite(wifiLedPin, LOW);
    }
}

void FeedbackManager::expressError(int code) {
    for (int i = 0; i < code; i++) {
        launchLedOn();
        if (buzzOnError) {
            motorOn(0);
            motorOff(800);
        }
        if (!defaultErrorAudio.isEmpty()) {
            playAudio(defaultErrorAudio.c_str());
        }
        launchLedOff(0, 400);
    }
}

void FeedbackManager::successActions(ZaparooToken* obj) {
    launchLedOn(0);
    const char* pathToPlay = obj->getLaunchAudio();
    if (pathToPlay == nullptr || strlen(pathToPlay) == 0) {
        pathToPlay = defaultLaunchAudio.c_str();
    }

    if (pathToPlay != nullptr && strlen(pathToPlay) > 0) {
        if (buzzOnLaunch) {
            motorOn(0);
            motorOff(100);
        }
        playAudio(pathToPlay);
    } else {
        if (buzzOnLaunch) {
            motorOn(0);
            motorOff(1000);
        }
    }
    launchLedOff(0, 0);
}

void FeedbackManager::setUidAudioMappings(ZaparooToken* obj) {
    if (!audioEnabled) return;
    File file;
    JsonDocument uidFile;

    if (sdCardEnabled && SD.exists(UID_MAP_FILE)) {
        file = SD.open(UID_MAP_FILE, FILE_READ);
    } else if (LittleFS.exists(UID_MAP_FILE)) {
        file = LittleFS.open(UID_MAP_FILE, "r");
    } else {
        return;
    }

    String uid = String(obj->getId());
    while (file.available()) {
        DeserializationError error = deserializeJson(uidFile, file);
        if (!error) {
            for (JsonObject item : uidFile["UID_ExtdRecs"].as<JsonArray>()) {
                if (item["UID"] == uid) {
                    obj->setLaunchAudio(item["launchAudio"].as<String>().c_str());
                    obj->setRemoveAudio(item["removeAudio"].as<String>().c_str());
                    file.close();
                    return;
                }
            }
        }
    }
    file.close();
}

void FeedbackManager::getUidMappings(JsonDocument& toSet){
  JsonDocument parsed;
  File file;
  toSet["msgType"] = "getUIDExtdRec";
  if(sdCardEnabled && SD.exists(UID_MAP_FILE)){
    file = SD.open(UID_MAP_FILE, FILE_READ);
  }else if(LittleFS.exists(UID_MAP_FILE)){
    file = LittleFS.open(UID_MAP_FILE, "r");
  }
  while (file.available()) { 
    DeserializationError error = deserializeJson(parsed, file);
    if(!error){
      toSet["data"] = parsed;
      file.close();
      return;
    }
  }
  file.close();
  return;
}

void FeedbackManager::saveUidMapping(JsonDocument &value){
  File file;
  if(sdCardEnabled && SD.exists(UID_MAP_FILE)){
    file = SD.open(UID_MAP_FILE, FILE_WRITE);
  }else if(LittleFS.exists(UID_MAP_FILE)){
    file = LittleFS.open(UID_MAP_FILE, FILE_WRITE);
  }
  while (file.available()) {
    serializeJson(value, file);
    file.close();
    return;
  }
  file.close();
  return;
}

int FeedbackManager::playAudio(const char* audioPath) {
    if (!audioEnabled || !audioPath || strlen(audioPath) == 0) {
        delay(1000);
        return 0;
    }

    AudioFileSource* file = nullptr;
    if (sdCardEnabled) {
        AudioFileSourceSD* source = new AudioFileSourceSD(audioPath);
        file = source;
    } else {
        AudioFileSourceLittleFS* source = new AudioFileSourceLittleFS(audioPath);
        file = source;
    }

    if (file->getSize() == 0) {
        delete file;
        file = nullptr;
        return 1;
    }

    AudioGeneratorMP3* mp3 = new AudioGeneratorMP3();
    mp3->begin(file, out);
    while (mp3->loop()) {}
    mp3->stop();

    delete mp3;
    mp3 = NULL;
    delete file;
    file = NULL;
    return 0;
}

void FeedbackManager::cardInsertedActions(ZaparooToken* obj) {
    const char* pathToPlay = obj->getDetectAudio();
    if (!pathToPlay || strlen(pathToPlay) == 0) {
        pathToPlay = defaultInsertAudio.c_str();
    }
    if (pathToPlay && strlen(pathToPlay) > 0) {
        playAudio(pathToPlay);
    }
    if (buzzOnDetect) {
        motorOn(0);
        motorOff(100);
    }
}

void FeedbackManager::cardRemovedActions(ZaparooToken* obj) {
    const char* pathToPlay = obj->getRemoveAudio();
    if (!pathToPlay || strlen(pathToPlay) == 0) {
        pathToPlay = defaultRemoveAudio.c_str();
    }
    if (pathToPlay && strlen(pathToPlay) > 0) {
        playAudio(pathToPlay);
    }
    if (buzzOnRemove) {
        motorOff();
    }
}
