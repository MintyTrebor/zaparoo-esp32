#pragma once
#include <TFT_eSPI.h>
#include "images/defBootImg.h"
#include <JPEGDecoder.h>
#include <ArduinoJson.h>
#include "images/connectingAP.h"
#include "images/connectingWiFi.h"
#include "images/gotoSleep.h"
#include "images/powerOff.h"
#include "images/setVolume.h"

class ScreenManager {
  private:
    void jpegRender(int xpos, int ypos);
    void drawSdJpeg(const char *filename, int xpos, int ypos);
    int rgbConvert(unsigned char r, unsigned char g, unsigned char b);
  public:
    const char *defaultImgPath = nullptr;
    const char *nowPlayingPath = nullptr;
    TFT_eSPI tftScr = TFT_eSPI();
    ScreenManager();
    ~ScreenManager();
    void init();
    void dispDefaultImg(const char* imgPath);
    void dispJpgImg(const char *imgPath);
    void dispAPConn();
    void dispWiFiConn();
    void dispNowPlaying();
    void dispGotoSleep();
    void dispPowerOff();
    void dispSetVolume();
    void screenSleep();
    void screenWake();
    void setNowPlayingPath(const char* nowPlayPath);
    void setDefImgPath(const char* defImgPath);
    void setScreenBacklight(bool backLight);
    void drawTextStr(String txtToShow, JsonDocument txtRGBJson, JsonDocument backGrdRGBJson, int fntSize, int fontNumber);
};