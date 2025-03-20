#pragma once
#include <RotaryEncoder.h>
#include <ArduinoJson.h>
#include "ScreenManager.h"
#include "FeedbackManager.h"
#include "PowerManager.h"
#include "UIDDataManager.h"
#include <ZaparooLaunchApi.h>

class InputManager {
  private:
    ScreenManager* screenManager;
    RotaryEncoder* encoder = nullptr;
    FeedbackManager* feedbackManager;
    PowerManager* powerManager;
    UIDDataManager* uidDataMan;
    JsonDocument currMenuJson;
    JsonDocument currMenuItemJson;
    ZaparooLaunchApi* ZapLaunchClient;
    bool isSerialOnly = false;
    String defSubMenuID = "";
    int currMenuItemCount = 0;
    String currMenuItemID = "";
    String IPSteam = "";
    String IPZap = "";
    int currMenuItemPos = 0;
    int lastRotationPos = 0;
    int lastScrnPos = 2;
    void getMainMenu(JsonDocument& menuJson);
    void getMenu(String menuID, JsonDocument& menuJson);
    void showCurrMenuItem();
    void doInputEventAudio(const char* aPath);
    bool sendToZap(String& gamePath);
  public:    
    InputManager();
    ~InputManager();    
    void init(ScreenManager* scrnMgr, RotaryEncoder* encdr, FeedbackManager* fbMan, PowerManager* pwrMan, UIDDataManager* UIDDMan, ZaparooLaunchApi* ZapCli, bool isSerial, String stmIP, String zpIP);
    void doRotaryButton();
    void doRotaryTurn(int currDir);
    void doDeepSleep();
    void setCurrMenu(String menuID);
    void setupMenu();
    void resetToDefaultMenu();
};