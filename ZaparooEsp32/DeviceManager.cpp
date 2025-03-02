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
  }
}

void DeviceManager::setupPins(String devType){

}
