#include <Arduino.h>
#include "..\ZaparooScanner.cpp"
#include "..\ZaparooToken.h"
#include <SecData.h>
#include <Wire.h>
#include "AudioFileSourceSD.h"
#include <PN532_I2C.h>
#include <PN532.h>
#include <FS.h>
#include <NfcAdapter.h>

class ZaparooPN532Scanner : public ZaparooScanner {
public:

    ZaparooPN532Scanner(){}

    ~ZaparooPN532Scanner() override {
      delete nfc;
    }

    bool init() override{
      if(nfc){
        delete nfc;
      }
      nfc = new NfcAdapter(*config);
      return nfc->begin();
    }

    bool tokenPresent() override {
      bool result = nfc->tagPresent();
      return result;
    }

    bool isNewToken() override{
      NfcTag tag = nfc->read();
      String id = tag.getUidString();
      id.replace(" ", "");
      if(id == ""){
        Serial.println("Id blank");
      }
      bool result = id != lastId;
      if(result){
        lastId = id;
      }
      return result;
    }

    ZaparooToken getToken() override {
        ZaparooToken token;
        NfcTag tag = nfc->read();
        String id = tag.getUidString();
        id.replace(" ", "");
        token.setId(id.c_str());
        if(tag.hasNdefMessage()){
          NdefMessage message = tag.getNdefMessage();
          int recordCount = message.getRecordCount();
          if(recordCount == 0) return token;
          token.setPayload(parseNdfMessage(message, token , 0));
          if(!token.getValid()){
            return token;
          }
          if(recordCount > 1){
            token.setLaunchAudio(parseNdfMessage(message, token , 1));
          }
          if(recordCount > 2){
            token.setRemoveAudio(parseNdfMessage(message, token , 2));
          }
        }
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


    void clearCache() override{
      lastId = "";
    }

    // Write a token to the given device
    bool writeLaunch(String& launchCmd, String& audioLaunchFile, String& audioRemoveFile) override {
        if (nfc->tagPresent()) {
          nfc->erase();
          NdefMessage message = NdefMessage();
          message.addTextRecord(launchCmd.c_str());
          if(audioLaunchFile.length() > 0){
            message.addTextRecord(audioLaunchFile.c_str());
          }
          if(audioRemoveFile.length() > 0){
            message.addTextRecord(audioRemoveFile.c_str());
          }
          bool success = nfc->write(message);
          if(!success){
            reset();
          }
          return success;
        }
        return false;
    }

    bool supportsWrite() const override {
        return true;
    }

    void setResetPin(int resetPin){
      this->resetPin = resetPin;
      pinMode(resetPin, OUTPUT);
      digitalWrite(resetPin, HIGH);
    }

    void setConfig(PN532_I2C& config){
      this->config = &config;
    }

private:
    int resetPin = -1;
    PN532_I2C* config = nullptr;
    String lastId = "";
    NfcAdapter* nfc = nullptr;

    const char* parseNdfMessage(NdefMessage& message, ZaparooToken& currentToken ,int recordIndex){
      NdefRecord record = message.getRecord(recordIndex);
      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);    
      String payloadAsString = "";
      for (int i = 3; i < payloadLength; i++) {
            int tmpIntChar = payload[i];
            if(!isAscii(tmpIntChar)){
              currentToken.setValid(false);
              return "";
            }
            payloadAsString += (char)payload[i];
      }
       return payloadAsString.c_str();
    } 
};
