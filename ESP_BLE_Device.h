#pragma once
#include <Arduino.h>
#include <BLEDevice.h>

#include "BLE_HID_Module.h"
#include "BLE_Smart_Module.h"

// System modes
enum SystemMode {
  MODE_OFF = 0,
  MODE_HID,
  MODE_SMART
};

class ESP_BLE_Device {
public:
  ESP_BLE_Device(const String& name = "ESP Device");

  // initialize BLE stack (no service started)
  void begin();

  // start/stop individual modes (restarts BLE stack internally)
  void startHID();      // advertise as HID mouse
  void startSmart();    // advertise as Smart device (TX/RX)
  void stop();          // fully stop BLE (MODE_OFF)

  // switch mode on the fly (graceful)
  void switchMode(SystemMode newMode);

  // helpers
  bool isConnected();   // true when a client is connected
  SystemMode getMode() const { return currentMode; }

  // HID API (when in HID mode)
  void hid_moveRel(int8_t dx, int8_t dy, int8_t wheel = 0, int8_t hWheel = 0);
  void hid_press(uint8_t button);
  void hid_release(uint8_t button);
  void hid_click(uint8_t button);
  void hid_scroll(int8_t amount);
  void hid_drag(uint8_t button, int8_t dx, int8_t dy, uint16_t holdMs = 30);

  // Smart mode API
  void smart_send(const String& key, const String& value);
  void smart_onReceive(void (*cb)(String key, String value));

private:
  String deviceName;
  SystemMode currentMode;

  // modules
  BLE_HID_Module hidModule;
  BLE_Smart_Module smartModule;

  // internal helpers
  void _stopBleStack(); // stops advertising, disconnects, deinit
  void _startCommon();  // helper to init BLEDevice
};
