#pragma once

#include <NimBLEDevice.h>

class ZaparooBleReporter {
public:
  explicit ZaparooBleReporter(const String &name);

  void setup();
  void sendString(const String &message);
  bool isConnected();

private:
  String deviceName;
  NimBLEServer* pServer = nullptr;
  NimBLECharacteristic* pCharacteristic = nullptr;
  NimBLEDescriptor* descriptor2901 = nullptr;

  bool deviceConnected = false;

  NimBLEUUID serviceUUID{"5a617061-726f-6f00-0000-000000000000"};
  NimBLEUUID characteristicUUID{"5a617061-726f-6f01-0000-000000000001"};
  const char* characteristicDescription = "Zaparoo BLE";

  class ServerCallbacks : public NimBLEServerCallbacks {
  public:
    explicit ServerCallbacks(ZaparooBleReporter& parent) : reporter(parent) {}

    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
      reporter.deviceConnected = true;
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
      reporter.deviceConnected = false;
      NimBLEDevice::startAdvertising();
    }

  private:
    ZaparooBleReporter& reporter;
  };

  class CharacteristicCallbacks : public NimBLECharacteristicCallbacks {
  public:
    explicit CharacteristicCallbacks(ZaparooBleReporter& parent) : reporter(parent) {}

    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {
      // Logging could go here
    }

  private:
    ZaparooBleReporter& reporter;
  };
};
