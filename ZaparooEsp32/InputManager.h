#pragma once
#include <RotaryEncoder.h>
#include "ScreenManager.h"

class InputManager {
  private:
    ScreenManager* screenManager;
    RotaryEncoder* encoder = nullptr;
    int lastRotationPos = 0;
    int lastScrnPos = 3;
  public:    
    InputManager();
    ~InputManager();    
    void init(ScreenManager* scrnMgr, RotaryEncoder* encdr);
    void nextRotation();
    void doRotaryButton();
    void doRotaryTurn();
};