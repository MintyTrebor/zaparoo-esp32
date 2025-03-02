#pragma once
#include <RotaryEncoder.h>
#include "ScreenManager.h"
#include "FeedbackManager.h"

class InputManager {
  private:
    ScreenManager* screenManager;
    RotaryEncoder* encoder = nullptr;
    FeedbackManager* feedbackManager;
    int lastRotationPos = 0;
    int lastScrnPos = 3;
  public:    
    InputManager();
    ~InputManager();    
    void init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan);
    void nextRotation();
    void doRotaryButton();
    void doRotaryTurn();
};