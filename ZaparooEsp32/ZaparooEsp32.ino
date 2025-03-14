#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LittleFS.h>
#include <ESPWebFileManager.h>
#include <NfcAdapter.h>
#include <ESPmDNS.h>
#include <ZaparooLaunchApi.h>
#include <ZaparooRestResult.h>
#include "index.h"
#include "ZaparooEsp32.hpp"
#include "ZaparooScanner.cpp"
#include "FeedbackManager.h"
#include "UIDDataManager.h"
#include "Update.h"
#include <ezButton.h>




#ifdef PN532
#include "scanners/ZaparooPN532Scanner.cpp"
PN532_I2C pn532_i2c(Wire);
String deviceType = "PN532";
#endif

#ifdef Lilygo
#include "scanners/ZaparooPN532Scanner.cpp"
#include "ScreenManager.h"
#include "InputManager.h"
#include "DeviceManager.h"
#include "PowerManager.h"
#include <RotaryEncoder.h>
#include <XPowersLib.h>
#include "bq27220.h"
PN532_I2C pn532_i2c(Wire);
String deviceType = "Lilygo";
ezButton rotaryButton(ENCODER_KEY);
ScreenManager scrnMan;
InputManager inpMan;
DeviceManager devMan;
PowerManager pwrMan;
XPowersPPM PPM;
BQ27220 bq27220;
BQ27220BatteryStatus bqBatt;
TaskHandle_t battery_handle;
const uint8_t i2c_sda = BOARD_I2C_SDA;
const uint8_t i2c_scl = BOARD_I2C_SCL;
bool pmu_ret = false;
RotaryEncoder encoder(ENCODER_INA, ENCODER_INB, RotaryEncoder::LatchMode::TWO03);
int lastRotPos = 0;
bool buttonTrigger;
#endif

#ifdef RC522
#include "scanners/ZaparooRC522Scanner.cpp"
MFRC522 mfrc522(BOARD_SD_CS, RST_PIN);
String deviceType = "RC522";
#endif

//Common Setup
ESPWebFileManager* fileManager;
Preferences preferences;
AsyncWebServer server(80);
AsyncWebSocket ws1("/ws");
ZaparooLaunchApi ZapClient;
ZaparooScanner* tokenScanner = NULL;
FeedbackManager feedback;
UIDDataManager UidDM;


//globals
String ZAP_URL = "ws://<replace>:7497" + String(ZaparooLaunchApi::wsPath);
ZaparooToken* token = NULL;
bool inserted = false;
bool isPN532 = false;
int timeoutLoop = 0;
bool isConnected = false;
bool isWebLog = false;
bool softReset = false;
bool zapEnabled = true;
bool steamEnabled = false;
bool uidScanMode= false;
bool serialOnly = false;
String steamIp = "steamOS.local";
String zapIp = "mister.local";
String lastSerialCommand = "";

//Prototypes
void notifyClients(const String& txtMsgToSend, const String& msgType);
void handleWebSocketMessage(void* arg, uint8_t* data, size_t len);
void doRotTurn(void);
void doRotButn(void);
void rotary(void *pvParameters);
void battery_task(void *pvParameters);

void setPref_Bool(const String& key, bool valBool) {
  preferences.putBool(key.c_str(), valBool);
}

void setPref_Int(const String& key, int valInt) {
  preferences.putInt(key.c_str(), valInt);
}

void setPref_Str(const String& key, const String& valStr) {
  preferences.putString(key.c_str(), valStr);
}

void setPref_Float(const String& key, float valFloat) {
  preferences.putFloat(key.c_str(), valFloat);
}

void restartESP(){
  Serial.println("Restarting ESP...");
  delay(100);
  ESP.restart();
}

void notifyClients(const String& txtMsgToSend, const String& msgType) {
  if(!serialOnly && txtMsgToSend != "KeepAlive") Serial.println(txtMsgToSend);
  if (isWebLog) {
    JsonDocument msgJson;
    msgJson["msgType"] = "notify";
    msgJson["data"]["msgTxt"] = txtMsgToSend;
    msgJson["data"]["type"] = msgType;
    String output;
    serializeJson(msgJson, output);
    delay(200);
    ws1.textAll(output.c_str());
  }
}

void cmdClients(JsonDocument& cmdJson) {
  String output;
  serializeJson(cmdJson, output);
  Serial.print("WS MSG SENT: ");
  Serial.println(output);
  delay(100);
  ws1.textAll(output.c_str());
}

void onEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type,
             void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      isWebLog = true;
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      setPref_Bool("enNfcWr", false);
      isWebLog = false;
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws1.onEvent(onEvent);
  server.addHandler(&ws1);
}

void startApMode() {
  server.end();
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP("zapesp32", "zapesp32");
  Serial.println("Starting AP Mode:");
  Serial.println(WiFi.softAPIP());
  feedback.doWiFiAP(deviceType);
  server.begin();
  initWebSocket();
  MDNS.begin("zapesp");
  Serial.flush();
  delay(100);
}

void connectWifi() {
  int i_strongest = -1;
  int32_t rssi_strongest = -100;
  if (WiFi.status() == WL_CONNECTED || WiFi.getMode() == WIFI_MODE_AP) {
    return;
  }
  WiFi.disconnect();
  MDNS.end();
  String ssid = preferences.getString("wifiSSID", "");
  if(ssid.isEmpty()){
    startApMode();
    return;
  }
  feedback.doWiFiConn(deviceType);
  //Code to connect esp32 to strongest AP in mesh network env
  int n = WiFi.scanNetworks();
  if (n != 0) {
    for (int i = 0; i < n; ++i) {
      if ((ssid == String(WiFi.SSID(i)) && (WiFi.RSSI(i)) > rssi_strongest)) {
        rssi_strongest = WiFi.RSSI(i);
        i_strongest = i;
      }
    }
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, preferences.getString("wifiPass", ""), 0, WiFi.BSSID(i_strongest));
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, preferences.getString("wifiPass", ""));
  }
  
  int retries = 30;
  while (WiFi.status() != WL_CONNECTED && retries--) {
    if(deviceType == "Lilygo"){
      feedback.lilygoWifiLed();
      delay(250);
      feedback.lilygoWifiLed();
    } else{
      feedback.wifiLedOn();
      delay(500);
      feedback.wifiLedOff();
    }    
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    startApMode();
    return;
  }
  // Initialize mDNS
  retries = 10;
  while (!MDNS.begin("zapesp") && retries--) {
    Serial.println("Error setting up MDNS responder!");
    delay(1000);
  }
  Serial.println("mDNS started - access the web UI @ http://zapesp.local");
  Serial.print("WiFi connected - Zap ESP IP = ");
  Serial.println(WiFi.localIP());
  WiFi.setSleep(false);
  server.begin();
  initWebSocket();
  feedback.motorOn();
  feedback.motorOff(250);
  feedback.motorOn(100);
  feedback.motorOff(250);
  feedback.wifiLedOn();
  feedback.doDefaultScreen(deviceType);
}

void writeTagLaunch(String& launchCmd, String& audioLaunchFile, String& audioRemoveFile, String& launchJPEGFile) {
  String tmpLaunchCmd = launchCmd;
  JsonDocument cmdData;
  cmdData["msgType"] = "writeResults";
  tmpLaunchCmd.replace("launch_cmd::", "");
  notifyClients("Launch Cmd Written: " + launchCmd, "log");
  if (tokenScanner->tokenPresent()) {
    bool success = tokenScanner->writeLaunch(launchCmd, audioLaunchFile, audioRemoveFile, launchJPEGFile);
    if (success) {
      cmdData["data"]["isSuccess"] = true;
      cmdData["data"]["isCardDetected"] = true;
      cmdClients(cmdData);
    } else {
      cmdData["data"]["isSuccess"] = false;
      cmdData["data"]["isCardDetected"] = true;
      cmdClients(cmdData);
    }
  } else {
    cmdData["data"]["isSuccess"] = false;
    cmdData["data"]["isCardDetected"] = false;
    cmdClients(cmdData);
  }  
  tokenScanner->halt();
}

//Load the UIDExtdRec.json and pass to Web Client
// void getUIDExtdRec(){
//   JsonDocument result;
//   feedback.getUidMappings(result);
//   cmdClients(result);
// }


bool send(String& gamePath) {
  String message;
  bool sent = false;
  if (serialOnly) {
    lastSerialCommand = "SCAN\ttext=" + gamePath;
    if(!feedback.resetOnRemove){
      lastSerialCommand = lastSerialCommand + "\tremovable=no";
      Serial.println(lastSerialCommand);
    }
    Serial.flush();
    message = "Sent game path to serial: " + gamePath;
    sent = true;
  } else {
    String newURL = ZAP_URL;
    newURL.replace("<replace>", gamePath.startsWith("steam://") ? steamIp : zapIp);
    ZapClient.url(newURL);
    notifyClients("URL: " + newURL, "log");
    int code = ZapClient.launch(gamePath);
    if (code > 0) {
      feedback.expressError(code);
      message = "Zaparoo Error Launching Game: " + gamePath + " | Error Code: " + code;
    } else {
      message = "Launched Game: " + gamePath;
      sent = true;
    }
  }
  notifyClients(message, "log");
  return sent;
}

bool sendUid(String& uid) {
  String message;
  bool sent = false;
  if (serialOnly) {
    lastSerialCommand = "SCAN\tuid=" + uid;
    if(!feedback.resetOnRemove){
      lastSerialCommand = lastSerialCommand + "\tremovable=no";
    }
    Serial.println(lastSerialCommand);
    Serial.flush();
    message = "Sent Card/Tag UID: " + uid;
    sent = true;
  } else {
    //not possible to determine if steam game from UID so always default to MiSTer if enabled
    String newURL = ZAP_URL;
    newURL.replace("<replace>", zapEnabled ? zapIp : steamIp);
    ZapClient.url(newURL);
    int code = ZapClient.launchUid(uid);
    if (code > 0) {
      feedback.expressError(code);
      message = "Zaparoo Error Sending Card/Tag UID " + uid + " | Error Code: " + code;
    } else {
      message = "Sent Card/Tag UID: " + uid;
      sent = true;
    }
  }
  notifyClients(message, "log");
  return sent;
}

//Send the detected UID to Web Client for Audio Mapping
void sendUIDtoWeb(String UIDStr){
  JsonDocument UIDData;
  JsonDocument fileData;
  UidDM.getUidFileJson(UIDStr.c_str(), fileData);
  UIDData["msgType"] = "pushedUIDFileJson";
  UIDData["data"]["UIDstr"] = UIDStr;
  UIDData["data"]["fileJson"] = fileData;
  cmdClients(UIDData);
}

void getWebConfigData() {
  notifyClients("Retrieving Current Zap ESP Config Data", "log");
  JsonDocument configData;
  feedback.set(configData);
  configData["msgType"] = "ConfigData";
  configData["data"]["zapIp"] = preferences.getString("zapIp", "mister.local");
  configData["data"]["zapEnabled"] = preferences.getBool("zapEnabled", true);
  configData["data"]["steamEnabled"] = preferences.getBool("steamEnabled", false);
  configData["data"]["serialOnly"] = preferences.getBool("serialOnly", false);
  configData["data"]["steamIp"] = preferences.getString("steamIp", "steamOS.local");
  configData["data"]["PN532_module"] = isPN532;
  configData["data"]["zapWsPath"] = ZaparooLaunchApi::wsPath;
  configData["data"]["deviceType"] = deviceType;
  cmdClients(configData);
}

void setWebConfigData(JsonDocument& cfgData) {
  notifyClients("ZAP ESP Now Saving Config Data", "log");
  feedback.launchLedOff();
  feedback.update(cfgData);
  if(cfgData["data"].containsKey("zapIp")){
    setPref_Str("zapIp", cfgData["data"]["zapIp"]);
  }
  if(cfgData["data"].containsKey("steamEnabled")){
    setPref_Bool("steamEnabled", cfgData["data"]["steamEnabled"]);
  }
  if(cfgData["data"].containsKey("zapEnabled")){
    setPref_Bool("zapEnabled", cfgData["data"]["zapEnabled"]);
  }
  if(cfgData["data"].containsKey("serialOnly")){
    setPref_Bool("serialOnly", cfgData["data"]["serialOnly"]);
  }
  if(cfgData["data"].containsKey("steamIp")){
    setPref_Str("steamIp", cfgData["data"]["steamIp"]);
  }
  preferences.end();
  Serial.println("Rebooting after saving");
  ESP.restart();
}

void handleResetRequest(){
  JsonDocument cmdData;
  cmdData["msgType"] = "closeWS";
  cmdClients(cmdData);
  delay(1000);
  ws1.closeAll();
  ws1.cleanupClients();
  ESP.restart();
}

void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
  AwsFrameInfo* info = static_cast<AwsFrameInfo*>(arg);
  if (!info->final || info->index != 0 || info->len != len || info->opcode != WS_TEXT) {
    return;  // Invalid frame
  }
  String webCmd(reinterpret_cast<char*>(data), len);
  JsonDocument root;
  DeserializationError error = deserializeJson(root, webCmd);
  if (error) {
    notifyClients("Failed to Parse JSON", "log");
    return;
  }
  String command = root["cmd"].as<String>();

  if (command == "set_WriteMode") {
    bool enableWriteMode = root["data"];
    notifyClients(enableWriteMode ? "NFC Tag Write Mode Enabled" : "NFC Tag Write Mode Disabled", "log");
    setPref_Bool("enNfcWr", enableWriteMode);
  } else if (command == "write_Tag_Launch_Game" && preferences.getBool("enNfcWr", false)) {
      notifyClients("NFC Tag Writing the Launch Game Command", "log");
      String launchData = root["data"]["launchData"].as<String>();
      String audioLaunchPath = root["data"]["audioLaunchPath"].as<String>();
      String audioRemovePath = root["data"]["audioRemovePath"].as<String>();
      String launchJPEGPath = root["data"]["launchJPEGPath"].as<String>();
      writeTagLaunch(launchData, audioLaunchPath, audioRemovePath, launchJPEGPath);
  } else if (command == "get_Current_Config") {
      getWebConfigData();
  } else if (command == "set_Current_Config") {
      setWebConfigData(root);
  } else if (command == "Test_Tag_Launch_Game") {
      notifyClients("Test Launching Game", "alert");
      String data = root["data"].as<String>();
      send(data);
  } else if (command == "closeWS") {
      setPref_Bool("enNfcWr", false);
      ws1.closeAll();
      ws1.cleanupClients();
  // } else if (command == "getUIDExtdRec") {
  //     notifyClients("Retrieving UIDExtdRec Data", "log");
  //     getUIDExtdRec();
  } else if (command == "set_UIDMode") {
      bool enableUIDMode = root["data"];
      notifyClients(enableUIDMode ? "UID Scanning Mode Enabled" : "UID Scanning Mode Disabled", "alert");
      uidScanMode = enableUIDMode;
      //get UIDExtdRec data if enabling UID mode
      // if(enableUIDMode){
      //   getUIDExtdRec();
      // }
  // } else if (command == "saveUIDExtdRec") {
  //     notifyClients("Saving UIDExtdRec Data", "log");
  //     JsonDocument data = root["data"];
  //     feedback.saveUidMapping(data);
  } else if (command == "wifi") {
    setPref_Str("wifiSSID", root["data"]["ssid"].as<String>());
    setPref_Str("wifiPass", root["data"]["password"].as<String>());
    WiFi.disconnect(); 
    WiFi.mode(WIFI_STA);
    notifyClients("Updated ssid", "log");
  } else if (command == "doReset") {
      handleResetRequest();
  } else if (command == "ping") {
      notifyClients("KeepAlive", "log");
  } else if (command == "saveUIDFileJson") {
      notifyClients("Saving UID File Json", "log");
      String tmpUID = root["data"]["UIDstr"].as<String>();
      JsonDocument data = root["data"]["fileJson"];
      UidDM.updateUidFileJson(tmpUID.c_str(), data);
  } else {
    notifyClients("Unknown Command", "log");
  }
}


void handleSend(){
  bool sent = false;
  bool playAudioFirst = serialOnly && feedback.resetOnRemove && !uidScanMode;
  feedback.setUidAudioMappings(token);
  if(playAudioFirst){
    feedback.successActions(token); //Play the audio before launch to support remove with simple serial
    sent = true;
  }
  if (!token->isBlankPayload() && !uidScanMode) {
    String payload = String(token->getPayload());
    sent = send(payload);
  } else if (token->isIdSet() && !uidScanMode) {
    String id = String(token->getId());
    sent = sendUid(id);
  }else if(token->isIdSet() && uidScanMode){
    String id = String(token->getId());
    sendUIDtoWeb(id);
  }
  if(!playAudioFirst && sent){
    feedback.successActions(token);
  }
}

//Loop 20 times per read to break out and run other loop code
bool readScanner() {
  for (int i = 0; i < 20 && !preferences.getBool("enNfcWr", false); i++) {
    bool present = tokenScanner->tokenPresent();
    ZaparooToken* parsed = present ? tokenScanner->getNewToken() : NULL;
    if (present && parsed) {
      if (!parsed->getValid()) {
        inserted = false;
        delete parsed;
        delay(10);
        continue;
      }
      delete token;
      token = NULL;
      token = parsed;
      feedback.cardInsertedActions(token);
      inserted = true;
      handleSend();
    } else if (!present && inserted) {  //Must have been removed
      String removeAudio = "";
      if (token->isRemoveAudioSet()) {
        removeAudio = token->getRemoveAudio();
      }
      feedback.cardRemovedActions(token);
      if (feedback.resetOnRemove && !serialOnly && token->isPayloadSet()) {
        String payloadAsString = String(token->getPayload());
        if (!payloadAsString.startsWith("steam://")) {
          ZapClient.stop();
        }
      }
      notifyClients("Tag Removed", "log");
      inserted = false;
      tokenScanner->halt();
      return true;
    }else if (present && inserted && serialOnly && feedback.resetOnRemove && !uidScanMode){
      Serial.println(lastSerialCommand);
      Serial.flush();
    }
    delay(10);
  }
  return false;
}

void setup() {
  Serial.begin(115200);
#ifdef Lilygo
  pinMode(DISPLAY_CS, OUTPUT);
  digitalWrite(DISPLAY_CS, HIGH);
  pinMode(BOARD_SD_CS, OUTPUT);
  digitalWrite(BOARD_SD_CS, HIGH);
  pinMode(BOARD_LORA_CS, OUTPUT);
  digitalWrite(BOARD_LORA_CS, HIGH);
  pinMode(BOARD_PWR_EN, OUTPUT);
  digitalWrite(BOARD_PWR_EN, HIGH);
  pinMode(BOARD_PN532_IRQ, OPEN_DRAIN);
  pinMode(BOARD_USER_KEY, INPUT);
  SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI);
  Wire.setPins(BOARD_I2C_SDA, BOARD_I2C_SCL);
  // iic scan
  uint8_t error, address;
  int nDevices = 0;
  bool pmu_ret = false;
  bool lora_ret = false;
  Serial.println("Scanning for I2C devices ...");
  Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
  for(address = 0x01; address < 0x7F; address++){
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if(error == 0){ // 0: success.
        nDevices++;
        if(address == BOARD_I2C_ADDR_2) {
            pmu_ret = true;
            Serial.println("I2C device found at BQ27220 address: " + String(address));
        }else if(address == BOARD_I2C_ADDR_3) {
            lora_ret = true;
            Serial.println("I2C device found at BQ25896 address: " + String(address));
        }
    }
  }
  if (nDevices == 0){
    Serial.println("No I2C devices found");
  }
  ZaparooPN532Scanner* pnScanner = new ZaparooPN532Scanner();
  pnScanner->setConfig(pn532_i2c);
  pnScanner->setResetPin(PN532_RST_PIN);
  tokenScanner = pnScanner;
  isPN532 = true;
  //Battery&PowerManagement
  if(pmu_ret){
    if(PPM.init(Wire, i2c_sda, i2c_scl, BOARD_I2C_ADDR_3)){
      Serial.println("Battery Initailising");
      pwrMan.init(&PPM);
      bq27220.init();
      xTaskCreate(battery_task, "battery_task", 1024 * 2, NULL, 4, &battery_handle);

    }
    else{
      Serial.println("Battery not Initailised");
    }
  }
#endif
#ifdef PN532
  SPI.begin();
  Wire.begin();
  ZaparooPN532Scanner* pnScanner = new ZaparooPN532Scanner();
  pnScanner->setConfig(pn532_i2c);
  pnScanner->setResetPin(PN532_RST_PIN);
  tokenScanner = pnScanner;
  isPN532 = true;
#endif
#ifdef RC522
  SPI.begin();
  ZaparooRC522Scanner* rcScanner = new ZaparooRC522Scanner();
  rcScanner->setConfig(mfrc522);
  mfrc522.PCD_Init();
  tokenScanner = rcScanner;
  isPN532 = false;
#endif
  //Check for PN532 Card Initalisation Failure and reset if in error
  if(!tokenScanner->init() && isPN532){
    tokenScanner->reset();
  }
  preferences.begin("qrplay", false);
  feedback.init(&preferences, deviceType, &devMan);
  UidDM.init(feedback.sdCardEnabled);
  feedback.initUidDataManager(&UidDM);
#ifdef Lilygo
  scrnMan.init();
  scrnMan.dispDefaultImg("");
  delay(1000);
  feedback.initScreen(&scrnMan);
  inpMan.init(&scrnMan, &encoder, &feedback, &pwrMan);
  pinMode(ENCODER_KEY, INPUT);  
  //attachInterrupt(ENCODER_KEY, doRotButn, FALLING);
  rotaryButton.setDebounceTime(100);
  attachInterrupt(digitalPinToInterrupt(ENCODER_INA), doRotTurn, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_INB), doRotTurn, CHANGE);
  xTaskCreatePinnedToCore(rotary, "rotary", 4096, NULL, 2, NULL,0);
#endif
  setPref_Bool("enNfcWr", false);
  uidScanMode= false;

  //set globals to reduce the number of call to preference library (performance)
  zapIp = preferences.getString("zapIp", "mister.local");
  zapEnabled = preferences.getBool("zapEnabled", true);
  steamEnabled = preferences.getBool("steamEnabled", false);
  steamIp = preferences.getString("steamIp", "steamOS.local");
  serialOnly = preferences.getBool("serialOnly", false);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", index_html, index_html_len);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
    int statusCode = Update.hasError() ? 500 : 200;
    request->send(statusCode);
    if (statusCode == 200) {
        Serial.println("Update successful. Restarting in 2s...");
        delay(500);
        xTaskCreate([](void *param) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);  // 2s delay
            restartESP();
            vTaskDelete(NULL);
        }, "restartTask", 2048, NULL, 1, NULL);
    }
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
        Serial.printf("Updating: %s\n", filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
            return;
        }
    }
    if (Update.write(data, len) != len) {
        Update.printError(Serial);
    }
    if (final) {
        if (!Update.end(true)) {
            Update.printError(Serial);
        } else {
            Serial.println("Update Success!");
        }
    }
  });

  if (feedback.sdCardEnabled) {
    Serial.println("SD CARD MODE");
    fileManager = new ESPWebFileManager(FS_SD, true);
  } else {
    Serial.println("LITTLEFS MODE");
    fileManager = new ESPWebFileManager(FS_LITTLEFS, true);
  }
  fileManager->setServer(&server);
  fileManager->begin();
  feedback.createUidMappingFile();
  UidDM.createUidDataDirectory();
}
#ifdef Lilygo
void doRotButn(void){
  buttonTrigger = true;
}
void doRotTurn(void){
  encoder.tick();
}
#endif

void loop() {
  connectWifi();
  if (!preferences.getBool("enNfcWr", false)) {
    readScanner();
  }  
  delay(50);
}
#ifdef Lilygo
void rotary(void *pvParameters) {
  while(1){
    rotaryButton.loop();
    int pos = 0;
    encoder.tick();
    int newPos = encoder.getPosition();
    if (pos != newPos) {
      inpMan.doRotaryTurn();
    }
    if(rotaryButton.isPressed()){
      //buttonTrigger = false;
      inpMan.doRotaryButton();
    }
    delay(50);
  }
}
void battery_task(void *pvParameters) {
  while(1){
    bq27220.getBatteryStatus(&bqBatt);
    Serial.println("Batt Status: " + String(bq27220.getStateOfCharge()));
    Serial.println("Batt Is Charging: " + String(bq27220.getIsCharging()));
    delay(10000);
  }
}
#endif
