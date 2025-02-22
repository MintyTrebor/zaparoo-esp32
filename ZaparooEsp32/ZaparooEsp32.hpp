#ifndef ZAPAROO_ESP32_H
  #define ZAPAROO_ESP32_H

  //*********************Reguired Configuration******************************
  
  //uncomment for RC522----------------------------------------------------
  //#define RC522
  //The SS/SDA and Reset pins of the RC522 Module using the default SPI GPIO pins of your ESP32
  //#define BOARD_SD_CS 5
  //#define RST_PIN 4
  //Default Audio Pins
  //#define BOARD_VOICE_BCLK 27
  //#define BOARD_VOICE_LRCLK 26
  //#define BOARD_VOICE_DIN 25
  //-----------------------------------------------------------------------
  
  //uncomment for PN532----------------------------------------------------
  //#define PN532
  //The SS/SDA and Reset pins of the SD CARD Module using the default SPI GPIO pins of your ESP32
  //#define BOARD_SD_CS 5
  //#define RST_PIN 4
  //The GPIO pin the PN532 RSTPDN Pin is connected to ((***NOT THE PN532 RSTO PIN***)). 
  //This is essential for stability and recovery after an esp32 soft reset.
  //#define PN532_RST_PIN 13
  //Default Audio Pins
  //#define BOARD_VOICE_BCLK 27
  //#define BOARD_VOICE_LRCLK 26
  //#define BOARD_VOICE_DIN 25
  //------------------------------------------------------------------------

  //------------------------------------------------------------------------
  //uncomment for Lilygo----------------------------------------------------
  #define Lilygo
  #define BOARD_PWR_EN   15
  #define BOARD_USER_KEY 6
  #define NUM_LEDS 8
  

  // WS2812
  #define WS2812_NUM_LEDS 8
  #define WS2812_DATA_PIN 14

  // IR
  #define BOARD_IR_EN 2
  #define BOARD_IR_RX 1

  // MIC
  #define BOARD_MIC_DATA 42
  #define BOARD_MIC_CLK  39

  // AUDIO
  #define BOARD_VOICE_BCLK  46
  #define BOARD_VOICE_LRCLK 40
  #define BOARD_VOICE_DIN   7

  // DISPLAY 
  #define DISPLAY_WIDTH  170
  #define DISPLAY_HEIGHT 320

  #define DISPLAY_BL   21 
  #define DISPLAY_CS   41
  #define DISPLAY_MISO 10
  #define DISPLAY_MOSI  9
  #define DISPLAY_SCLK 11
  #define DISPLAY_DC   16
  #define DISPLAY_RST  40

  // --------- ENCODER ---------
  #define ENCODER_INA 4
  #define ENCODER_INB 5
  #define ENCODER_KEY 0

  // --------- IIC ---------
  #define BOARD_I2C_SDA  8
  #define BOARD_I2C_SCL  18

  // IIC addr
  #define BOARD_I2C_ADDR_1 0x24  // PN532
  #define BOARD_I2C_ADDR_2 0x55  // BQ27220
  #define BOARD_I2C_ADDR_3 0x6b  // BQ25896

  // NFC
  #define BOARD_PN532_SCL     BOARD_I2C_SCL
  #define BOARD_PN532_SDA     BOARD_I2C_SDA
  #define BOARD_PN532_RF_REST 45
  #define BOARD_PN532_IRQ     17

  // --------- SPI ---------
  #define BOARD_SPI_SCK  11
  #define BOARD_SPI_MOSI 9
  #define BOARD_SPI_MISO 10

  // TF card
  #define BOARD_SD_CS   13
  #define BOARD_SD_SCK  BOARD_SPI_SCK
  #define BOARD_SD_MOSI BOARD_SPI_MOSI
  #define BOARD_SD_MISO BOARD_SPI_MISO

  // LORA
  #define BOARD_LORA_CS   12
  #define BOARD_LORA_SCK  BOARD_SPI_SCK
  #define BOARD_LORA_MOSI BOARD_SPI_MOSI
  #define BOARD_LORA_MISO BOARD_SPI_MISO
  #define BOARD_LORA_IO2  38
  #define BOARD_LORA_IO0  3
  #define BOARD_LORA_SW1  47
  #define BOARD_LORA_SW0  48
  //-----------------------------------------------------------------------
  
  #define PN532_RST_PIN BOARD_PN532_RF_REST
  #define LED_DATA_PIN WS2812_DATA_PIN

#endif


//INFO ------------------------------------------------------------------------
//DEFAULT ESP32S3 GPIO PIN ASSIGNMENTS
// PN532 Module
// (Check your ESP32's default I2C pins as the code uses them by default)
// SDA            8
// SCL            9
// RSTPDN         36 (Can be reassigned above with PN532_RST_PIN)

// SDCARD or RC522 Module
// (Check your ESP32's default vspi pins as the code uses them by default)
// SDA            10 (Can be reassigned above with SS_PIN)
// SCK            12
// MOSI           11
// MISO           13
// RST            4 (Can be reassigned above with RST_PIN)

// LED's          !!Configurable in Web UI!!
// WIFI           2 (This is normally replicates the cards wifi led, but it may be a different gpio on your esp32)
// PWR            37
// Launch         39

// Rumble Motor   !!Configurable in Web UI!!
// Motor          35

// Audio MAX98357 Module   !!Configurable in Web UI!!
// DOUT           17
// BCLK           16
// LRC            15
// GAIN           GND

//INFO ------------------------------------------------------------------------
//DEFAULT ESP32S2 Mini GPIO PIN ASSIGNMENTS
// PN532 Module
// (Check your ESP32's default I2C pins as the code uses them by default)
// SDA            8
// SCL            9
// RSTPDN         12 (Can be reassigned above with PN532_RST_PIN)

// SDCARD or RC522 Module
// (Check your ESP32's default vspi pins as the code uses them by default)
// SDA            34 (Can be reassigned above with SS_PIN)
// SCK            36
// MOSI           35
// MISO           34
// RST            4 (Can be reassigned above with RST_PIN)

// LED's          !!Configurable in Web UI!!
// WIFI           11 (This is normally replicates the cards wifi led, but it may be a different gpio on your esp32)
// PWR            6
// Launch         4

// Rumble Motor   !!Configurable in Web UI!!
// Motor          11

// Audio MAX98357 Module   !!Configurable in Web UI!!
// DOUT           17
// BCLK           16
// LRC            15
// GAIN           GND

//INFO ------------------------------------------------------------------------
//DEFAULT ESP32 GPIO PIN ASSIGNMENTS
// PN532 Module
// (Check your ESP32's default I2C pins as the code uses them by default)
// SDA            21
// SCL            22
// RSTPDN         13 (Can be reassigned above with PN532_RST_PIN)

// SDCARD or RC522 Module
// (Check your ESP32's default vspi pins as the code uses them by default)
// SDA            5 (Can be reassigned above with SS_PIN)
// SCK            18
// MOSI           23
// MISO           19
// RST            4 (Can be reassigned above with RST_PIN)

// LED's          !!Configurable in Web UI!!
// WIFI           2 (This is normally replicates the cards wifi led, but it may be a different gpio on your esp32)
// PWR            15
// Launch         33

// Rumble Motor   !!Configurable in Web UI!!
// Motor          32

// Audio MAX98357 Module   !!Configurable in Web UI!!
// DOUT           25
// BCLK           27
// LRC            26
// GAIN           GND
