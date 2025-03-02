#pragma once
#include <TFT_eSPI.h>
#include "images/defBootImg.h"
#include <JPEGDecoder.h>
#include "images/onePlayerStartImg.h"
#include "images/twoPlayerStartImg.h"
#include "images/insertCoinImg.h"

class ScreenManager {
  private:
    void jpegRender(int xpos, int ypos);
    void drawSdJpeg(const char *filename, int xpos, int ypos);
  public:
    const char *defaultImgPath = nullptr;
    const char *nowPlayingPath = nullptr;
    TFT_eSPI tftScr = TFT_eSPI();
    ScreenManager();
    ~ScreenManager();
    void init();
    void dispDefaultImg(const char* imgPath);
    void dispJpgImg(const char *imgPath);
    void disp1PStart();
    void disp2PStart();
    void dispInsCoin();
    void dispNowPlaying();
    void setNowPlayingPath(const char* nowPlayPath);
    void setDefImgPath(const char* defImgPath);
};