#pragma once
#include "ZaparooEsp32.hpp"
#include "PowerManager.h"

PowerManager::PowerManager() {
}

PowerManager::~PowerManager() {
}

void PowerManager::init(XPowersPPM* xPPM){
  PPM = xPPM;
  // Set the minimum operating voltage. Below this voltage, the PPM will protect
  PPM->setSysPowerDownVoltage(3300);
  // Set input current limit, default is 500mA
  PPM->setInputCurrentLimit(3250);
  Serial.printf("getInputCurrentLimit: %d mA\n",PPM->getInputCurrentLimit());
  // Disable current limit pin
  PPM->disableCurrentLimitPin();
  // Set the charging target voltage, Range:3840 ~ 4608mV ,step:16 mV
  PPM->setChargeTargetVoltage(4208);
  // Set the precharge current , Range: 64mA ~ 1024mA ,step:64mA
  PPM->setPrechargeCurr(64);
  // The premise is that Limit Pin is disabled, or it will only follow the maximum charging current set by Limi tPin.
  // Set the charging current , Range:0~5056mA ,step:64mA
  PPM->setChargerConstantCurr(832);
  // Get the set charging current
  PPM->getChargerConstantCurr();
  Serial.printf("getChargerConstantCurr: %d mA\n",PPM->getChargerConstantCurr());
  // To obtain voltage data, the ADC must be enabled first
  PPM->enableADCMeasure();    
  // Turn on charging function
  // If there is no battery connected, do not turn on the charging function
  
  Serial.println("BC " + String(PPM->isBatteryConnect()));
  if(PPM->isBatteryConnect()){
    Serial.println("Battery Connected");
    PPM->enableCharge();
    // The OTG function needs to enable OTG, and set the OTG control pin to HIGH
    // After OTG is enabled, if an external power supply is plugged in, OTG will be turned off

    PPM->enableOTG();
    PPM->disableOTG();
    // Turn off charging function
    // If USB is used as the only power input, it is best to turn off the charging function, 
    // otherwise the VSYS power supply will have a sawtooth wave, affecting the discharge output capability.
    // PPM.disableCharge();
  }else{
    Serial.println("Battery Not Connected");
  }

}

bool PowerManager::isBattConnected(){
  return PPM->isBatteryConnect();
}

void PowerManager::initCharging(){
  PPM->enableCharge();
  // The OTG function needs to enable OTG, and set the OTG control pin to HIGH
  // After OTG is enabled, if an external power supply is plugged in, OTG will be turned off
  PPM->enableOTG();
  PPM->disableOTG();
    
}

void PowerManager::stopCharging(){
  // Turn off charging function
  // If USB is used as the only power input, it is best to turn off the charging function, 
  // otherwise the VSYS power supply will have a sawtooth wave, affecting the discharge output capability.
  PPM->disableCharge();
}