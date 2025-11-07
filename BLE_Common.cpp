#include "BLE_Common.h"

BLEServer* BLECommon_createServer(const char* name) {
  if (!BLEDevice::getInitialized()) {
    BLEDevice::init(name);
  }
  BLEServer* server = BLEDevice::createServer();
  return server;
}
