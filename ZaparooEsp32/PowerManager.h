#pragma once
#include <XPowersLib.h>

class PowerManager {
  private:
    XPowersPPM* PPM = nullptr;
  public:    
    PowerManager();
    ~PowerManager();
    void init(XPowersPPM* xPPM);
    bool isBattConnected();
    void initCharging();
    void stopCharging();
};