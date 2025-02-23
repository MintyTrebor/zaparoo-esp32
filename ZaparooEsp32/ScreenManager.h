#pragma once
#include "libraries/TFT_eSPI/TFT_eSPI.h"
#include "defBootImg.h"
#include <JPEGDecoder.h>
#include "onePlayerStartImg.h"
#include "twoPlayerStartImg.h"
#include "insertCoinImg.h"

class ScreenManager {
  private:
    void jpegRender(int xpos, int ypos);
    void drawSdJpeg(const char *filename, int xpos, int ypos);
  public:
    String defaultImgPath = "";
    TFT_eSPI tftScr = TFT_eSPI();
    ScreenManager();
    ~ScreenManager();
    void init();
    void dispDefaultImg(String imgPath);
    void dispJpgImg(const char *imgPath);
    void disp1PStart();
    void disp2PStart();
    void dispInsCoin();
};