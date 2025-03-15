#pragma once
#include <RotaryEncoder.h>
#include <ArduinoJson.h>
#include "ScreenManager.h"
#include "FeedbackManager.h"
#include "PowerManager.h"

class InputManager {
  private:
    ScreenManager* screenManager;
    RotaryEncoder* encoder = nullptr;
    FeedbackManager* feedbackManager;
    PowerManager* powerManager;
    JsonDocument mJson;
    int currMenuItemCount = 0;
    int currMenuItemID = 0;
    int currMenuItemPos = 0;
    int lastRotationPos = 0;
    int lastScrnPos = 2;
    void getMainMenu(JsonDocument& menuJson);
    void nextRotation();
    void previousRotation();
  public:    
    InputManager();
    ~InputManager();    
    void init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan, PowerManager* pwrMan);
    void doRotaryButton();
    void doRotaryTurn(int currDir);
    void doDeepSleep();
};