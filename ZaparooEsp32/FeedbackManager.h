#pragma once
#include "Audio.h"
#include <LittleFS.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include "ZaparooToken.h"
#include "ZaparooEsp32.hpp"
#include <FastLED.h>
#include "DeviceManager.h"
#include "UIDDataManager.h"

#ifdef Lilygo
#include "ScreenManager.h"
#endif

class FeedbackManager {
private:
    Preferences* preferences;
    DeviceManager* devMangr;
    UIDDataManager* UidDMan;
    #ifdef Lilygo
    ScreenManager* screenManager;
    #endif
    void setupPins();
    void ledRingRed();
    void ledRingBlue();
    void ledRingGreen();
    void ledRingOff();
    
public:
    float audioGain = 21.0;
    bool wifiLedEnabled = false;
    bool motorEnabled = false;
    bool launchLedEnabled = false;
    bool audioEnabled = false;
    bool pwrLedEnabled = false;
    bool resetOnRemove = true;
    bool sdCardEnabled = false;
    bool buzzOnDetect = true;
    bool buzzOnLaunch = true;
    bool buzzOnRemove = true;
    bool buzzOnError = true;

    String defaultInsertAudio = "";
    String defaultLaunchAudio = "";
    String defaultRemoveAudio = "";
    String defaultErrorAudio = "";
    String defaultImgPath = "";
    String deviceType = "";

    int motorPin = -1;
    int launchLedPin = -1;
    int wifiLedPin = -1;
    int pwrLedPin = -1;
    int i2sBclkPin = -1;
    int i2sLrcPin = -1;
    int i2sDoutPin = -1;

    FeedbackManager();
    ~FeedbackManager();
    void init(Preferences* prefs, String devType, DeviceManager* devMan);
    void update(JsonDocument& doc);
    void set(JsonDocument& doc);
    void motorOn(int predelay = 0);
    void motorOff(int predelay = 0);
    void launchLedOn(int predelay = 0);
    void launchLedOff(int predelay = 0, int postDelay = 0);
    void wifiLedOn();
    void wifiLedOff();
    void expressError(int code);
    void successActions(ZaparooToken* obj);
    void setUidMappings(ZaparooToken* obj);
    int playAudio(const char* audioPath);
    void cardInsertedActions(ZaparooToken* obj);
    void cardRemovedActions(ZaparooToken* obj);
    void lilygoWifiLed();
    void createUidMappingFile();
    void doWiFiAP(String devType);
    void doWiFiConn(String devType);
    void doDefaultScreen(String devType);
    void initUidDataManager(UIDDataManager* UidDM); 
    #ifdef Lilygo
    void initScreen(ScreenManager* scrnMgr);
    void doDoze(bool doze);
    void ledRingSleep();
    #endif
     
};
