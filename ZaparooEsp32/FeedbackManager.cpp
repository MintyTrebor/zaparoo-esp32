#include "FeedbackManager.h"
#include "ZaparooEsp32.hpp"

#define UID_MAP_FILE "/uidExtdRecord.json"


FeedbackManager::FeedbackManager() {
}

FeedbackManager::~FeedbackManager() {
}

//Prefences has a 14 character limit for key
void FeedbackManager::init(Preferences* prefs, String devType, DeviceManager* devMan) {
    preferences = prefs;
    JsonDocument defaultData;
    devMangr = devMan;
    devMangr->setDeviceDefaults(devType);
    devMangr->getDeviceDefaults(defaultData);

    audioGain = preferences->getFloat("audioGain", defaultData["data"]["audioGain"].as<float>());
    wifiLedEnabled = preferences->getBool("wifiLed", defaultData["data"]["wifiLedEnabled"].as<bool>());
    motorEnabled = preferences->getBool("motor", defaultData["data"]["motorEnabled"].as<bool>());
    launchLedEnabled = preferences->getBool("launchLed", defaultData["data"]["launchLedEnabled"].as<bool>());
    audioEnabled = preferences->getBool("audio", defaultData["data"]["audioEnabled"].as<bool>());
    pwrLedEnabled = preferences->getBool("pwrLed", defaultData["data"]["pwrLedEnabled"].as<bool>());
    resetOnRemove = preferences->getBool("resetOnRemove", defaultData["data"]["resetOnRemove"].as<bool>());
    sdCardEnabled = preferences->getBool("sdCard", defaultData["data"]["sdCardEnabled"].as<bool>());
    buzzOnDetect = preferences->getBool("buzzOnDetect", defaultData["data"]["buzzOnDetect"].as<bool>());
    buzzOnLaunch = preferences->getBool("buzzOnLaunch", defaultData["data"]["buzzOnLaunch"].as<bool>());
    buzzOnRemove = preferences->getBool("buzzOnRemove", defaultData["data"]["buzzOnRemove"].as<bool>());
    buzzOnError = preferences->getBool("buzzOnError", defaultData["data"]["buzzOnError"].as<bool>());
    defaultInsertAudio = preferences->getString("insertAudio", defaultData["data"]["defaultInsertAudio"].as<String>());
    defaultLaunchAudio = preferences->getString("launchAudio", defaultData["data"]["defaultLaunchAudio"].as<String>());
    defaultRemoveAudio = preferences->getString("removeAudio", defaultData["data"]["defaultRemoveAudio"].as<String>());
    defaultErrorAudio = preferences->getString("errorAudio", defaultData["data"]["defaultErrorAudio"].as<String>());
    defaultImgPath = preferences->getString("defImgPath", defaultData["data"]["defaultImgPath"].as<String>());
    deviceType = devType;
    
    motorPin = preferences->getInt("motorPin", defaultData["data"]["motorPin"].as<int>());
    launchLedPin = preferences->getInt("launchLedPin", defaultData["data"]["launchLedPin"].as<int>());
    wifiLedPin = preferences->getInt("wifiLedPin", defaultData["data"]["wifiLedPin"].as<int>());
    pwrLedPin = preferences->getInt("pwrLedPin", defaultData["data"]["pwrLedPin"].as<int>());

    // Read the I2S pins from preferences but don't save them in member variables
    i2sBclkPin = preferences->getInt("i2sBclkPin", defaultData["data"]["i2sBclkPin"].as<int>());
    i2sLrcPin = preferences->getInt("i2sLrcPin", defaultData["data"]["i2sLrcPin"].as<int>());
    i2sDoutPin = preferences->getInt("i2sDoutPin", defaultData["data"]["i2sDoutPin"].as<int>());

    setupPins();
    delay(500);
    if(deviceType == "Lilygo"){
      //Overriding any user Audio Pin settings for this device as they should never be changed by the user
      i2sBclkPin = defaultData["data"]["i2sBclkPin"].as<int>();
      i2sLrcPin = defaultData["data"]["i2sLrcPin"].as<int>();
      i2sDoutPin = defaultData["data"]["i2sDoutPin"].as<int>();
    }
}
#ifdef Lilygo
void FeedbackManager::initScreen(ScreenManager* scrnMgr) {
  screenManager = scrnMgr;
  screenManager->setDefImgPath(defaultImgPath.c_str());
}

void FeedbackManager::doDoze(bool doze){
  if(doze){
    if(pwrLedEnabled){
      CRGB leds[NUM_LEDS];
      FastLED.addLeds<WS2813, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
      FastLED.setBrightness(5);
      for(int i = 0; i < NUM_LEDS; i++){
          leds[i] = CRGB::Purple;
      }
      FastLED.show();
    }
    screenManager->setScreenBacklight(false);
  }else{
    if(pwrLedEnabled){
      ledRingRed();
    }
    screenManager->setScreenBacklight(true);
  }
}
#endif

void FeedbackManager::initUidDataManager(UIDDataManager* UidDM) {
  UidDMan = UidDM;
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

void FeedbackManager::doWiFiAP(String devType){
  if(devType == "Lilygo"){
#ifdef Lilygo
    screenManager->dispAPConn();
#endif
  }
}

void FeedbackManager::doWiFiConn(String devType){
  if(devType == "Lilygo"){
#ifdef Lilygo
    screenManager->dispWiFiConn();
#endif
  }
}

void FeedbackManager::doDefaultScreen(String devType){
  if(devType == "Lilygo"){
#ifdef Lilygo
    screenManager->dispDefaultImg(defaultImgPath.c_str());
#endif
  }
}

void FeedbackManager::setupPins() {
  if (motorEnabled) {
    pinMode(motorPin, OUTPUT);
  }
  if (wifiLedEnabled) {
    if(deviceType != "Lilygo"){pinMode(wifiLedPin, OUTPUT);}
  }
  if (launchLedEnabled) {
    if(deviceType != "Lilygo"){pinMode(launchLedPin, OUTPUT);}
  }
  if (pwrLedEnabled) {
    if(deviceType != "Lilygo"){
      pinMode(pwrLedPin, OUTPUT);
      digitalWrite(pwrLedPin, HIGH);
    }
    if(deviceType == "Lilygo"){
      ledRingRed();
    }
  }
  if (sdCardEnabled) {
    if (!SD.begin(BOARD_SD_CS)) {
      Serial.println(F("failed to do SD Card"));
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
    if (doc["data"].containsKey("defaultImgPath")) {
        defaultImgPath = doc["data"]["defaultImgPath"].as<String>();
        preferences->putString("defImgPath", defaultImgPath);
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
    doc["data"]["defaultImgPath"] = defaultImgPath;
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
    if (launchLedEnabled && deviceType != "Lilygo") {
        delay(predelay);
        digitalWrite(launchLedPin, HIGH);
    }
    if (launchLedEnabled && deviceType == "Lilygo") {
        delay(predelay);
        ledRingGreen();
    }
}

void FeedbackManager::launchLedOff(int predelay, int postDelay) {
    if (launchLedEnabled && deviceType != "Lilygo") {
      delay(predelay);
      digitalWrite(launchLedPin, LOW);
      delay(postDelay);
    }
    if (launchLedEnabled && deviceType == "Lilygo") {
      delay(predelay);
      if (pwrLedEnabled) {
        ledRingRed();
      }else {
        ledRingOff();
      }
      delay(postDelay);
    }
}

void FeedbackManager::wifiLedOn() {
    if (wifiLedEnabled && deviceType != "Lilygo") {
        digitalWrite(wifiLedPin, HIGH);
    }
}

void FeedbackManager::wifiLedOff() {
    if (wifiLedEnabled && deviceType != "Lilygo") {
        digitalWrite(wifiLedPin, LOW);
    }
}

void FeedbackManager::lilygoWifiLed() {
  CRGB leds[NUM_LEDS];
  FastLED.addLeds<WS2813, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = CRGB::Blue;
    for(int j = i+1; j < NUM_LEDS; j++){
      leds[j] = CRGB::Red;
    }
    FastLED.show();
    delay(50);
  }
  delay(100);
  if(pwrLedEnabled){
    ledRingRed(); 
  }
  else {
    ledRingOff();
  }
}

void FeedbackManager::ledRingSleep() {
    CRGB leds[NUM_LEDS];
    FastLED.addLeds<WS2813, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(10);
    for(int j = 0; j < NUM_LEDS; j++){
        leds[j] = CRGB::Black;
    }
    leds[0] = CRGB::Red;
    leds[2] = CRGB::Red;
    leds[4] = CRGB::Red;
    leds[6] = CRGB::Red;
    FastLED.show();
}

void FeedbackManager::ledRingRed() {
    CRGB leds[NUM_LEDS];
    FastLED.addLeds<WS2813, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = CRGB::Red;
    }
    FastLED.show();
}

void FeedbackManager::ledRingGreen() {
    CRGB leds[NUM_LEDS];
    FastLED.addLeds<WS2813, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB::Green;
      for(int j = i+1; j < NUM_LEDS; j++){
        leds[j] = CRGB::Black;
      }
      FastLED.show();
      delay(50);
    }
    
}

void FeedbackManager::ledRingBlue() {
    CRGB leds[NUM_LEDS];
    FastLED.addLeds<WS2813, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = CRGB::Blue;
    }
    FastLED.show();
}

void FeedbackManager::ledRingOff() {
    CRGB leds[NUM_LEDS];
    FastLED.addLeds<WS2813, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(0);
    for(int i = 0; i < NUM_LEDS; i++){
        leds[i] = CRGB::Black;
    }
    FastLED.show();
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
#ifdef Lilygo
    if(deviceType == "Lilygo"){
      screenManager->dispDefaultImg(defaultImgPath.c_str());
      screenManager->setNowPlayingPath("");
    }
#endif
    }
}

void FeedbackManager::successActions(ZaparooToken* obj) {
    launchLedOn(0);
    
#ifdef Lilygo
    if(deviceType == "Lilygo" && obj->isLaunchJPEGSet()){
      const char* imgToShow = obj->getLaunchJPEG();
      if (imgToShow || strlen(imgToShow) > 0) {
        screenManager->dispJpgImg(imgToShow);
        screenManager->setNowPlayingPath(imgToShow);
      }
    }else if(deviceType == "Lilygo"){
      screenManager->setNowPlayingPath(nullptr);
      screenManager->dispDefaultImg(defaultImgPath.c_str());
    }
#endif

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

void FeedbackManager::setUidMappings(ZaparooToken* obj) {
    if (!audioEnabled) return;
    JsonDocument uidFile;
    bool isExistingFile = false;
    UidDMan->getUidFileJson(obj->getId(), uidFile, isExistingFile);
    if(isExistingFile){
      obj->setLaunchAudio(uidFile["cmds"][0]["args"]["client"][0]["args"]["audio"]["launchAudioPath"].as<String>().c_str());
      obj->setRemoveAudio(uidFile["cmds"][0]["args"]["client"][0]["args"]["audio"]["removeAudioPath"].as<String>().c_str());
      obj->setLaunchJPEG(uidFile["cmds"][0]["args"]["client"][0]["args"]["display"]["imgPath"].as<String>().c_str()); 
    }
    if(uidFile["launchImg"].as<String>().length() > 0){
      screenManager->setNowPlayingPath(uidFile["cmds"][0]["args"]["client"][0]["args"]["display"]["imgPath"].as<String>().c_str());
    }else {
      screenManager->setNowPlayingPath(nullptr);
      //screenManager->dispDefaultImg(defaultImgPath.c_str());
    }
}


int FeedbackManager::playAudio(const char* audioPath) {
    if (!audioEnabled || !audioPath || strlen(audioPath) == 0) {
        delay(1000);
        return 0;
    }
    Audio audio;  
    audio.setPinout(i2sBclkPin, i2sLrcPin, i2sDoutPin);
    audio.setVolume(audioGain);
    if (sdCardEnabled) {
        audio.connecttoFS(SD, audioPath);
    } else {
        audio.connecttoFS(SPIFFS, audioPath);
    }
    audio.loop();
    while(audio.isRunning()){
      audio.loop();
      vTaskDelay(1);
    }
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
#ifdef Lilygo
    if(deviceType == "Lilygo" && resetOnRemove){
      screenManager->dispDefaultImg(defaultImgPath.c_str());
      screenManager->setNowPlayingPath("");
    }
#endif
}
