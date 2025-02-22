#pragma once
#include <Arduino.h>
#include "ZaparooToken.h"

class ZaparooScanner {
public:
    // Virtual destructor to ensure proper cleanup of derived classes
    virtual ~ZaparooScanner() {}

    //Call during program intialization
    virtual bool init() = 0;

    //Called once each time the server enters the read loop (loop terminats on token removal)
    virtual bool begin(){return true;};

    // Check if a token is present (e.g., if an NFC card is on the reader)     
    virtual bool tokenPresent() = 0;

    // Called after a token is removed
    virtual void halt() {}

    // Contains the logic for reading the token
    virtual ZaparooToken* getNewToken() = 0;

    // Reset the device reading the token
    virtual void reset() {}

    // Clear any cached variables, used when a invalid scan is detected
    virtual void clearCache(){}

    // Write a token to the given device
    virtual bool writeLaunch(String& launchCmd, String& audioLaunchFile, String& audioRemoveFile, String& launchJPEGFile) {return false;}

    // Returns true if the device supports writing
    virtual bool supportsWrite() const { return false; }
};
