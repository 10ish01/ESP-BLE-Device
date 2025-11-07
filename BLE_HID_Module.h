#pragma once
#include <Arduino.h>
#include <BLEHIDDevice.h>
#include <HIDTypes.h>
#include "BleConnectionStatus.h"

#define HID_BTN_LEFT   0x01
#define HID_BTN_RIGHT  0x02
#define HID_BTN_MIDDLE 0x04

class BLE_HID_Module {
public:
  BLE_HID_Module();

  void begin(const char* deviceName);
  void stop();

  bool isConnected();

  // HID actions
  void moveRel(int8_t dx, int8_t dy, int8_t wheel = 0, int8_t hWheel = 0);
  void press(uint8_t button);
  void release(uint8_t button);
  void click(uint8_t button);
  void scroll(int8_t amount);

private:
  void _initHID(BLEServer* server);

  BLEHIDDevice* hid = nullptr;
  BLECharacteristic* inputMouse = nullptr;
  BLECharacteristic* inputAbsMouse = nullptr;
  BleConnectionStatus connStatus;
  uint8_t buttons = 0;
};
