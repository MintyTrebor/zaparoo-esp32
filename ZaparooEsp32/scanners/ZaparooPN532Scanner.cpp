#include <Arduino.h>
#include "../ZaparooScanner.cpp"
#include "../ZaparooToken.h"
#include <SecData.h>
#include <Wire.h>
//#include "AudioFileSourceSD.h"
#include <PN532_I2C.h>
#include <PN532.h>
#include <FS.h>
#include <NfcAdapter.h>

class ZaparooPN532Scanner : public ZaparooScanner {
public:

  ZaparooPN532Scanner() {}

  ~ZaparooPN532Scanner() override {
    delete nfc;
  }

  bool init() override {
    if (nfc) {
      delete nfc;
    }
    nfc = new NfcAdapter(*config);
    return nfc->begin();
  }

  bool tokenPresent() override {
    bool result = nfc->tagPresent();
    return result;
  }

  ZaparooToken* getNewToken() override {
    String id = getTagUID();
    if (id == lastId) {
      return NULL;
    }
    NfcTag tag = nfc->read();
    ZaparooToken* token = new ZaparooToken();
    token->setId(id.c_str());
    if (tag.hasNdefMessage()) {
      NdefMessage message = tag.getNdefMessage();
      int recordCount = message.getRecordCount();
      if (recordCount != 0) {
        token->setPayload(parseNdfMessage(message, token, 0).c_str());
        if (!token->getValid()) {
          return token;
        }
        if (recordCount > 1) {
          token->setLaunchAudio(parseNdfMessage(message, token, 1).c_str());
        }
        if (recordCount > 2) {
          token->setRemoveAudio(parseNdfMessage(message, token, 2).c_str());
        }
        if (recordCount > 3) {
          token->setLaunchJPEG(parseNdfMessage(message, token, 3).c_str());
        }
      }
    }
    lastId = id;  //Only set on valid reads
    return token;
  }

  void halt() override {
    clearCache();
  }

  void reset() override {
    digitalWrite(resetPin, LOW);
    delay(100);
    digitalWrite(resetPin, HIGH);
    init();
  }


  void clearCache() override {
    lastId.clear();
  }

  // Write a token to the given device
  bool writeLaunch(String& launchCmd, String& audioLaunchFile, String& audioRemoveFile, String& launchJPEGFile) override {
    if (nfc->tagPresent()) {
      if(nfc->erase()){
        NdefMessage message = NdefMessage();
        message.addTextRecord(launchCmd.c_str());
        if (audioLaunchFile.length() > 0) {
          message.addTextRecord(audioLaunchFile.c_str());
        } else {
          message.addTextRecord("");
        }
        if (audioRemoveFile.length() > 0) {
          message.addTextRecord(audioRemoveFile.c_str());
        } else {
          message.addTextRecord("");
        }
        if (launchJPEGFile.length() > 0) {
          message.addTextRecord(launchJPEGFile.c_str());
        } else {
          message.addTextRecord("");
        }
        bool success = nfc->write(message);
        if (!success) {
          reset();
        }
        return success;
      }
    }
    return false;
  }

  bool supportsWrite() const override {
    return true;
  }

  void setResetPin(int resetPin) {
    this->resetPin = resetPin;
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
  }

  void setConfig(PN532_I2C& config) {
    this->config = &config;
  }

private:
  int resetPin = -1;
  PN532_I2C* config = nullptr;
  String lastId = "";
  NfcAdapter* nfc = nullptr;

  const String parseNdfMessage(NdefMessage& message, ZaparooToken* currentToken, int recordIndex) {
    NdefRecord record = message.getRecord(recordIndex);
    int payloadLength = record.getPayloadLength();
    uint8_t payload[payloadLength];
    record.getPayload(payload);
    String payloadAsString = "";
    for (int i = 3; i < payloadLength; i++) {
      int tmpIntChar = payload[i];
      if (!isAscii(tmpIntChar)) {
        currentToken->setValid(false);
        return "";
      }
      payloadAsString += (char)payload[i];
    }
    return payloadAsString;
  }

  String getTagUID() {
    uint8_t* uid = nfc->uid;
    uint8_t uidLength = nfc->uidLength;
    char uidString[2 * uidLength + 1] = { 0 };
    for (unsigned int i = 0; i < uidLength; i++) {
      char hex[3];
      if (uid[i] < 0x10) {
        snprintf(hex, sizeof(hex), "0%X", uid[i]);
      } else {
        snprintf(hex, sizeof(hex), "%X", uid[i]);
      }
      strcat(uidString, hex);
    }
    String result = String(uidString);
    result.toLowerCase();
    return result;
  }
};
