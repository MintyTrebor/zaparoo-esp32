#include "ZaparooEsp32.hpp"
#include "DeviceManager.h"

DeviceManager::DeviceManager() {
}

DeviceManager::~DeviceManager() {
}

void DeviceManager::setDeviceDefaults(String devType){
  //onlychange if defaults are diifferent from safe defaults
  if(devType == "Lilygo"){
      launchLedEnabled = true;
      audioEnabled = true;
      pwrLedEnabled = true;
      resetOnRemove = true;
      i2sBclkPin = 46;
      i2sLrcPin = 40;
      i2sDoutPin = 7;       
  }
}

void DeviceManager::setupPins(String devType){

}

void DeviceManager::getDeviceDefaults(JsonDocument& doc) {
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