#pragma once
#include <RotaryEncoder.h>
#include "ScreenManager.h"

class InputManager {
  private:
    ScreenManager* screenManager;
    int lastRotationPos = 2;
  public:    
    InputManager();
    ~InputManager();
    void init(ScreenManager* scrnMgr);
    void nextRotation();
};