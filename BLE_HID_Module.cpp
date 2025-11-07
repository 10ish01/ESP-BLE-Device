#include "BLE_HID_Module.h"
#include "BLE_Common.h"

// Report ID
#define REPORT_ID_REL  0x01

// Combined HID report map (relative mouse only)
static const uint8_t _hidReportDescriptor[] = {
  0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
  0x09, 0x02,       // USAGE (Mouse)
  0xA1, 0x01,       // COLLECTION (Application)
  0x85, REPORT_ID_REL, // REPORT_ID (1)
  0x09, 0x01,       //   USAGE (Pointer)
  0xA1, 0x00,       //   COLLECTION (Physical)
    0x05, 0x09,       //     USAGE_PAGE (Button)
    0x19, 0x01,       //     USAGE_MINIMUM (Button 1)
    0x29, 0x05,       //     USAGE_MAXIMUM (Button 5)
    0x15, 0x00,       //     LOGICAL_MINIMUM (0)
    0x25, 0x01,       //     LOGICAL_MAXIMUM (1)
    0x95, 0x05,       //     REPORT_COUNT (5)
    0x75, 0x01,       //     REPORT_SIZE (1)
    0x81, 0x02,       //     INPUT (Data,Var,Abs)
    0x95, 0x01,       //     REPORT_COUNT (1)
    0x75, 0x03,       //     REPORT_SIZE (3)
    0x81, 0x03,       //     INPUT (Const,Var,Abs)
    0x05, 0x01,       //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,       //     USAGE (X)
    0x09, 0x31,       //     USAGE (Y)
    0x09, 0x38,       //     USAGE (Wheel)
    0x15, 0x81,       //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,       //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,       //     REPORT_SIZE (8)
    0x95, 0x03,       //     REPORT_COUNT (3)
    0x81, 0x06,       //     INPUT (Data,Var,Rel)
  0xC0,             //   END_COLLECTION
  0xC0              // END_COLLECTION
};

BLE_HID_Module::BLE_HID_Module() {}

void BLE_HID_Module::begin(const char* deviceName) {
  BLEServer* server = BLECommon_createServer(deviceName);
  server->setCallbacks(&connStatus);

  // Create HID device
  hid = new BLEHIDDevice(server);
  hid->manufacturer()->setValue("ESP Unified");
  hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
  hid->hidInfo(0x00, 0x01);

  // Input report
  inputMouse = hid->inputReport(REPORT_ID_REL);
  connStatus.inputMouse = inputMouse;

  // Report map setup
  hid->reportMap((uint8_t*)_hidReportDescriptor, sizeof(_hidReportDescriptor));
  hid->startServices();

  BLEAdvertising* adv = server->getAdvertising();
  adv->setAppearance(HID_MOUSE);
  adv->addServiceUUID(hid->hidService()->getUUID());
  adv->start();
}

void BLE_HID_Module::stop() {
  if (hid) {
    BLEServer* server = BLEDevice::getServer();
    if (server) {
      server->getAdvertising()->stop();
    }
    hid = nullptr;
    inputMouse = nullptr;
  }
}

bool BLE_HID_Module::isConnected() {
  return connStatus.connected;
}

void BLE_HID_Module::moveRel(int8_t dx, int8_t dy, int8_t wheel, int8_t hWheel) {
  if (!isConnected() || !inputMouse) return;
  uint8_t rep[5];
  rep[0] = buttons;
  rep[1] = (uint8_t)dx;
  rep[2] = (uint8_t)dy;
  rep[3] = (uint8_t)wheel;
  rep[4] = (uint8_t)hWheel;
  inputMouse->setValue(rep, sizeof(rep));
  inputMouse->notify();
}

void BLE_HID_Module::press(uint8_t button) {
  buttons |= button;
  moveRel(0,0,0,0);
}

void BLE_HID_Module::release(uint8_t button) {
  buttons &= ~button;
  moveRel(0,0,0,0);
}

void BLE_HID_Module::click(uint8_t button) {
  press(button);
  delay(20);
  release(button);
}

void BLE_HID_Module::scroll(int8_t amount) {
  moveRel(0,0,amount,0);
}
