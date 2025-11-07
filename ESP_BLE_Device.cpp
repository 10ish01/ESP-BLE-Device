#include "ESP_BLE_Device.h"

ESP_BLE_Device::ESP_BLE_Device(const String& name) :
  deviceName(name), currentMode(MODE_OFF)
{
}

void ESP_BLE_Device::begin() {
  // do not init services here; modules will call BLEDevice::init when needed
  // but initialize a default state if desired
  currentMode = MODE_OFF;
}

void ESP_BLE_Device::_stopBleStack() {
  // Stop advertising, disconnect clients and deinit BLE
  if (BLEDevice::getInitialized()) {
    BLEServer* server = BLEDevice::getServer();
    if (server) {
      if (server->getConnectedCount() > 0) {
        // disconnect all (safe attempt)
        // there is no direct disconnectAll, but we can stop advertising
        server->getAdvertising()->stop();
      } else {
        server->getAdvertising()->stop();
      }
    }
    // small delay then deinit
    delay(100);
    BLEDevice::deinit(false);
  }
}

void ESP_BLE_Device::_startCommon() {
  // initialize BLE stack with name
  if (!BLEDevice::getInitialized()) {
    BLEDevice::init(deviceName.c_str());
  }
}

void ESP_BLE_Device::stop() {
  // stop currently running module (if any) and deinit BLE
  if (currentMode == MODE_HID) {
    hidModule.stop();
  } else if (currentMode == MODE_SMART) {
    smartModule.stop();
  }
  _stopBleStack();
  currentMode = MODE_OFF;
}

void ESP_BLE_Device::startHID() {
  if (currentMode == MODE_HID) return;
  stop();
  _startCommon();
  hidModule.begin(deviceName.c_str());
  currentMode = MODE_HID;
}

void ESP_BLE_Device::startSmart() {
  if (currentMode == MODE_SMART) return;
  stop();
  _startCommon();
  smartModule.begin(deviceName.c_str());
  currentMode = MODE_SMART;
}

void ESP_BLE_Device::switchMode(SystemMode newMode) {
  if (newMode == currentMode) return;
  Serial.printf("[ESP_BLE_Device] switching %d -> %d\n", currentMode, newMode);
  if (newMode == MODE_OFF) {
    stop();
    return;
  }
  if (newMode == MODE_HID) startHID();
  else if (newMode == MODE_SMART) startSmart();
}

bool ESP_BLE_Device::isConnected() {
  if (currentMode == MODE_HID) return hidModule.isConnected();
  if (currentMode == MODE_SMART) return smartModule.isConnected();
  return false;
}

// HID wrappers
void ESP_BLE_Device::hid_moveRel(int8_t dx, int8_t dy, int8_t wheel, int8_t hWheel) {
  if (currentMode == MODE_HID) hidModule.moveRel(dx, dy, wheel, hWheel);
}
void ESP_BLE_Device::hid_press(uint8_t button) {
  if (currentMode == MODE_HID) hidModule.press(button);
}
void ESP_BLE_Device::hid_release(uint8_t button) {
  if (currentMode == MODE_HID) hidModule.release(button);
}
void ESP_BLE_Device::hid_click(uint8_t button) {
  if (currentMode == MODE_HID) hidModule.click(button);
}
void ESP_BLE_Device::hid_scroll(int8_t amount) {
  if (currentMode == MODE_HID) hidModule.scroll(amount);
}
void ESP_BLE_Device::hid_drag(uint8_t button, int8_t dx, int8_t dy, uint16_t holdMs) {
  if (currentMode == MODE_HID) {
    hidModule.press(button);
    hidModule.moveRel(dx, dy);
    delay(holdMs);
    hidModule.release(button);
  }
}

// Smart wrappers
void ESP_BLE_Device::smart_send(const String& key, const String& value) {
  if (currentMode == MODE_SMART) smartModule.send(key, value);
}
void ESP_BLE_Device::smart_onReceive(void (*cb)(String, String)) {
  smartModule.setReceiveCallback(cb);
}
