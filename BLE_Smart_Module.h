#pragma once
#include <Arduino.h>
#include <BLEDevice.h>
#include "BleConnectionStatus.h"

class BLE_Smart_Module {
public:
  BLE_Smart_Module();
  void begin(const char* deviceName);
  void stop();
  bool isConnected();

  // send key:value (not JSON, simple colon separated)
  void send(const String& key, const String& value);

  // receive callback setter
  void setReceiveCallback(void (*cb)(String key, String value));
  BleConnectionStatus connStatus;
private:
  
  BLECharacteristic* txChar = nullptr; // notify
  BLECharacteristic* rxChar = nullptr; // write
};
