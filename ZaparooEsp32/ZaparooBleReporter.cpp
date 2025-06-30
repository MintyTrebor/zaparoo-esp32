#include "ZaparooBleReporter.h"

ZaparooBleReporter::ZaparooBleReporter(const String &name) : deviceName(name) {}

void ZaparooBleReporter::setup() {
  NimBLEDevice::init(std::string(deviceName.c_str()));
  NimBLEDevice::setMTU(247); // 517 max, 247 for BLE 4.2 compatibility

  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks(*this));

  NimBLEService* pService = pServer->createService(serviceUUID);

  pCharacteristic = pService->createCharacteristic(
    characteristicUUID,
    NIMBLE_PROPERTY::READ |
    NIMBLE_PROPERTY::WRITE |
    NIMBLE_PROPERTY::NOTIFY |
    NIMBLE_PROPERTY::INDICATE
  );

  descriptor2901 = pCharacteristic->createDescriptor(
    NimBLEUUID((uint16_t)0x2901),
    NIMBLE_PROPERTY::READ
  );
  descriptor2901->setValue(characteristicDescription);

  NimBLE2904* desc2904 = pCharacteristic->create2904();
  desc2904->setFormat(NimBLE2904::FORMAT_UTF8);

  pCharacteristic->setCallbacks(new CharacteristicCallbacks(*this));
  pService->start();

  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serviceUUID);
  pAdvertising->enableScanResponse(true);
  pAdvertising->setPreferredParams(0x06, 0x12);
  pAdvertising->setName(deviceName.c_str());

  NimBLEDevice::startAdvertising();

  Serial.println("[DEBUG] BLE advertising started");
}

void ZaparooBleReporter::sendString(const String &message) {
  if (!deviceConnected || !pCharacteristic) return;

  const int chunkPayloadSize = 150;
  int msgLen = message.length();
  int totalChunks = (msgLen + chunkPayloadSize - 1) / chunkPayloadSize;

  for (int i = 0; i < totalChunks; ++i) {
    int offset = i * chunkPayloadSize;
    int len = (msgLen - offset > chunkPayloadSize) ? chunkPayloadSize : (msgLen - offset);
    String chunkBody = message.substring(offset, offset + len);

    // Format header: [01/05], zero-padded
    char header[10];
    snprintf(header, sizeof(header), "[%02d/%02d]", i + 1, totalChunks);

    // Combine header and chunk body
    String chunk = String(header) + chunkBody;

    pCharacteristic->setValue(chunk.c_str());
    pCharacteristic->notify();

    delay(10);  // Give the client time to process
  }
}


bool ZaparooBleReporter::isConnected() {
  return deviceConnected;
}
