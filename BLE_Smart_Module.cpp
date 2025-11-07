#include "BLE_Smart_Module.h"
#include "BLE_Common.h"

static const char* SMART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static const char* SMART_RX_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"; // write (phone -> esp)
static const char* SMART_TX_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"; // notify (esp -> phone)

class SmartCallbacks : public BLECharacteristicCallbacks {
public:
  SmartCallbacks(BLE_Smart_Module* parent) : _parent(parent) {}
  void onWrite(BLECharacteristic* characteristic) override {
   std::string val = std::string(characteristic->getValue().c_str());

    if (val.empty()) return;
    // parse "key:value"
    size_t sep = val.find(':');
    if (sep == std::string::npos) return;
    String key = String(val.substr(0, sep).c_str());
    String value = String(val.substr(sep + 1).c_str());

    if (_parent && _parent->connStatus.connected) {
    if (_parent->connStatus.inputMouse == nullptr) {
        

        // noop
      }
    }
    // call user callback via module (we'll store in module static via setter)
    if (_receiveCb) _receiveCb(key, value);
  }

  static void (*_receiveCb)(String, String);

private:
  BLE_Smart_Module* _parent;
};
void (*SmartCallbacks::_receiveCb)(String, String) = nullptr;

BLE_Smart_Module::BLE_Smart_Module() {}

void BLE_Smart_Module::begin(const char* deviceName) {
  BLEServer* server = BLECommon_createServer(deviceName);
  server->setCallbacks(&connStatus);

  BLEService* svc = server->createService(SMART_SERVICE_UUID);

  txChar = svc->createCharacteristic(
    SMART_TX_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );
  txChar->addDescriptor(new BLE2902());

  rxChar = svc->createCharacteristic(
    SMART_RX_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  // set callbacks to handle writes
  SmartCallbacks* cb = new SmartCallbacks(this);
  SmartCallbacks::_receiveCb = nullptr; // ensure cleared; set via setReceiveCallback
  rxChar->setCallbacks(cb);

  // keep ref in connStatus so we can enable notifications on connect
  connStatus.txCharacteristic = txChar;

  svc->start();

  BLEAdvertising* adv = server->getAdvertising();
  adv->addServiceUUID(svc->getUUID());
  adv->start();
}

void BLE_Smart_Module::stop() {
  if (BLEDevice::getInitialized()) {
    BLEServer* server = BLEDevice::getServer();
    if (server) server->getAdvertising()->stop();
  }
  txChar = nullptr;
  rxChar = nullptr;
}

bool BLE_Smart_Module::isConnected() {
  return connStatus.connected;
}

void BLE_Smart_Module::send(const String& key, const String& value) {
  if (!txChar || !isConnected()) return;
  String msg = key + ":" + value;
  txChar->setValue((uint8_t*)msg.c_str(), msg.length());
  txChar->notify();
}

void BLE_Smart_Module::setReceiveCallback(void (*cb)(String, String)) {
  SmartCallbacks::_receiveCb = cb;
}
