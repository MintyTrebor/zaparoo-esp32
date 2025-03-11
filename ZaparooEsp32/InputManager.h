#pragma once
#include <RotaryEncoder.h>
#include "ScreenManager.h"
#include "FeedbackManager.h"
#include "PowerManager.h"

class InputManager {
  private:
    ScreenManager* screenManager;
    RotaryEncoder* encoder = nullptr;
    FeedbackManager* feedbackManager;
    PowerManager* powerManager;
    int lastRotationPos = 0;
    int lastScrnPos = 3;
  public:    
    InputManager();
    ~InputManager();    
    void init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan, PowerManager* pwrMan);
    void nextRotation();
    void doRotaryButton();
    void doRotaryTurn();
    void doDeepSleep();
};