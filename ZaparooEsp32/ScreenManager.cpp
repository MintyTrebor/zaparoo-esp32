#include "ScreenManager.h"
#include "ZaparooEsp32.hpp"

ScreenManager::ScreenManager() {
}

ScreenManager::~ScreenManager() {
}

//Prefences has a 14 character limit for key
void ScreenManager::init(){
  tftScr.begin();
  tftScr.begin();// Don't know why this has to be done twice but it works!!
  delay(500);
  //dispDefaultImg(defaultImgPath);
}

void ScreenManager::dispDefaultImg(String imgPath){
  const char* imgToShow = imgPath.c_str();
  tftScr.setRotation(0);
  tftScr.fillScreen(TFT_WHITE);
  if (imgToShow == nullptr || strlen(imgToShow) == 0) {
    bool decoded = JpegDec.decodeArray(ZapEspBootLogo, 12545);
    if(decoded) {
      jpegRender(0, 0);
    }
    return;
  }
  if (SD.exists(imgToShow)) {
    drawSdJpeg(imgToShow, 0, 0);
  } else {
    return;
  }  
}

void ScreenManager::disp1PStart(){
  bool decoded = JpegDec.decodeArray(PlayerOneStart_jpg, 24024);
  if(decoded) {
    jpegRender(0, 0);
  }
}
void ScreenManager::disp2PStart(){
  bool decoded = JpegDec.decodeArray(PlayerTwoStart_jpg, 24097);
  if(decoded) {
    jpegRender(0, 0);
  }
}
void ScreenManager::dispInsCoin(){
  bool decoded = JpegDec.decodeArray(InsertCoin_jpg, 9028);
  if(decoded) {
    jpegRender(0, 0);
  }
}

void ScreenManager::dispJpgImg(const char *imgPath){
  //const char* imgToShow = imgPath.c_str();
  if (imgPath == nullptr || strlen(imgPath) == 0) {
    return;
  }
  if (SD.exists(imgPath)) {
    tftScr.fillScreen(TFT_WHITE);
    drawSdJpeg(imgPath, 0, 0);
  } else {
    return;
  }
}

void ScreenManager::drawSdJpeg(const char *filename, int xpos, int ypos) {
  // Open the named file (the Jpeg decoder library will close it)
  File jpegFile = SD.open(filename, FILE_READ);  // or, file handle reference for SD library 
  if ( !jpegFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }
  bool decoded = JpegDec.decodeSdFile(jpegFile);  // Pass the SD file handle to the decoder,
  if (decoded) {
    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

void ScreenManager::jpegRender(int xpos, int ypos) {

  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  bool swapBytes = tftScr.getSwapBytes();
  tftScr.setSwapBytes(true);  
  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
  uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);
  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;
  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();
  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;
  // Fetch data from the file, decode and display
  while (JpegDec.read()) {    // While there is more data in the file
    pImg = JpegDec.pImage ;   // Decode a MCU (Minimum Coding Unit, typically a 8x8 or 16x16 pixel block)
    // Calculate coordinates of top left corner of current MCU
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;
    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;
    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }
    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;
    // draw image MCU block only if it will fit on the screen
    if (( mcu_x + win_w ) <= tftScr.width() && ( mcu_y + win_h ) <= tftScr.height())
      tftScr.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    else if ( (mcu_y + win_h) >= tftScr.height())
      JpegDec.abort(); // Image has run off bottom of screen so abort decoding
  }
  tftScr.setSwapBytes(swapBytes);
}