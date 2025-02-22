#include <Arduino.h>
#include "../ZaparooScanner.cpp"
#include "../ZaparooToken.h"
#include <MFRC522.h>

#include <NfcAdapter.h>

class ZaparooRC522Scanner : public ZaparooScanner {
public:

    ZaparooRC522Scanner(){}

    ~ZaparooRC522Scanner() override {
      delete nfc;
    }

    bool init() override{
      if(nfc){
        delete nfc;
      }
      nfc = new NfcAdapter(config);
      nfc->begin();
      return true;
    }

    bool tokenPresent() override {
      bool result = nfc->tagPresent();
      if(!result){
        //tagPresent returns true once, until a new tag is added. These ensure old tag is present
        result = config->PICC_IsNewCardPresent() || config->PICC_ReadCardSerial();
      }
      return result;
    }

    //There was an attempt to use the result of tokenPresent to determine this
    //The blank id bug requires it to be done this way
    bool isNewToken(){
      if(config->uid.size == 0){ //Blank id
        reset();
        nfc->tagPresent();
      }
      String id = getUidString();
      bool result = id != lastId;
      if(result){
        lastId = id;
      }
      return result;
    }

    ZaparooToken* getNewToken() override {
		if(!isNewToken()){
			return NULL;
		}
        ZaparooToken* token = new ZaparooToken();
        NfcTag tag = nfc->read();
        String id = getUidString();
        token->setId(id.c_str());
        if(tag.hasNdefMessage()){
          NdefMessage message = tag.getNdefMessage();
          int recordCount = message.getRecordCount();
          if(recordCount == 0){
            return token;
          }
          token->setPayload(parseNdfMessage(message, token , 0).c_str());
          if(!token->getValid()){
            return token;
          }
          if(recordCount > 1){
            token->setLaunchAudio(parseNdfMessage(message, token , 1).c_str());
          }
          if(recordCount > 2){
            token->setRemoveAudio(parseNdfMessage(message, token , 2).c_str());
          }
          if (recordCount > 3) {
            token->setLaunchJPEG(parseNdfMessage(message, token, 3).c_str());
          }
        }
        return token;
    }

    void halt() override {
        clearCache();
        config->uid.size = 0; //Issue where id is not read, so not reset in the lib. This ensures the reader can be reset in the isNewToken and no misreads are processed
        delay(50);
    }

    void reset(){
        nfc->haltTag();
        config->PCD_Reset();
        config->PCD_Init();
    }

    void clearCache() override{
      lastId.clear();
    }

    // Write a token to the given device
    bool writeLaunch(String& launchCmd, String& audioLaunchFile, String& audioRemoveFile, String& launchJPEGFile) override {
		if (nfc->tagPresent()) {
          if(nfc->erase()){
            NdefMessage message = NdefMessage();
            message.addTextRecord(launchCmd.c_str());
            if(audioLaunchFile.length() > 0){
              message.addTextRecord(audioLaunchFile.c_str());
            } else {
              message.addTextRecord("");
            }
            if(audioRemoveFile.length() > 0){
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
            if(!success){
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

    void setConfig(MFRC522& config){
      this->config = &config;
    }

private:
    MFRC522* config=NULL;
    String lastId = "";
    NfcAdapter* nfc = NULL;

    const String parseNdfMessage(NdefMessage& message, ZaparooToken* currentToken ,int recordIndex){
      NdefRecord record = message.getRecord(recordIndex);
      int payloadLength = record.getPayloadLength();
      const byte *payload = record.getPayload();
      String payloadAsString = "";
      for (int i = 3; i < payloadLength; i++) {
            int tmpIntChar = payload[i];
            if(!isAscii(tmpIntChar)){
              currentToken->setValid(false);
              return "";
            }
            payloadAsString += (char)payload[i];
      }
      payloadAsString.trim();
      return payloadAsString;
    } 

    String getUidString() {
      byte* uid = config->uid.uidByte;
      uint8_t uidLength = config->uid.size;
      char uidString[2 * uidLength + 1] = {0};
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
