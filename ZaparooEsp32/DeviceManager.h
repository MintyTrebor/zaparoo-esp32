#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

class DeviceManager {
  private:
    //safe defaults
    int audioGain = 21;
    bool wifiLedEnabled = true;
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
    int motorPin = 32;
    int launchLedPin = 33;
    int wifiLedPin = 2;
    int pwrLedPin = 15;
    int i2sBclkPin = BOARD_VOICE_BCLK;
    int i2sLrcPin = BOARD_VOICE_LRCLK;
    int i2sDoutPin = BOARD_VOICE_DIN;   
  public:    
    DeviceManager();
    ~DeviceManager();
    void setDeviceDefaults(String devType);
    void setupPins(String devType);
    void getDeviceDefaults(JsonDocument& doc);
};