#pragma once
#include <BLEServer.h>
#include <BLE2902.h>

// Simple connection status handler shared across modules
class BleConnectionStatus : public BLEServerCallbacks {
public:
  bool connected = false;

  // optional ptrs to descriptors / characteristics so we can enable notifications
  BLECharacteristic* inputMouse = nullptr;
  BLECharacteristic* inputAbsMouse = nullptr;
  BLECharacteristic* txCharacteristic = nullptr;

  void onConnect(BLEServer* pServer) override {
    connected = true;
    // enable notifications descriptors if present
    if (inputMouse) {
      BLE2902* d = (BLE2902*)inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
      if (d) d->setNotifications(true);
    }
    if (inputAbsMouse) {
      BLE2902* d = (BLE2902*)inputAbsMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
      if (d) d->setNotifications(true);
    }
    if (txCharacteristic) {
      BLE2902* d = (BLE2902*)txCharacteristic->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
      if (d) d->setNotifications(true);
    }
  }

  void onDisconnect(BLEServer* pServer) override {
    connected = false;
    // restart advertising so host can reconnect
    BLEAdvertising* adv = pServer->getAdvertising();
    if (adv) adv->start();
  }
};
