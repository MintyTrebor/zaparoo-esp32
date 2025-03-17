#pragma once
#include <RotaryEncoder.h>
#include <ArduinoJson.h>
#include "ScreenManager.h"
#include "FeedbackManager.h"
#include "PowerManager.h"
#include "UIDDataManager.h"

class InputManager {
  private:
    ScreenManager* screenManager;
    RotaryEncoder* encoder = nullptr;
    FeedbackManager* feedbackManager;
    PowerManager* powerManager;
    UIDDataManager* uidDataMan;
    JsonDocument currMenuJson;
    JsonDocument currMenuItemJson;
    int currMenuItemCount = 0;
    String currMenuItemID = "";
    int currMenuItemPos = 0;
    int lastRotationPos = 0;
    int lastScrnPos = 2;
    void getMainMenu(JsonDocument& menuJson);
    void getMenu(String menuID, JsonDocument& menuJson);
    void doCurrMenuItem();
  public:    
    InputManager();
    ~InputManager();    
    void init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan, PowerManager* pwrMan, UIDDataManager* UIDDMan);
    void doRotaryButton();
    void doRotaryTurn(int currDir);
    void doDeepSleep();
    void setCurrMenu(String menuID);
};