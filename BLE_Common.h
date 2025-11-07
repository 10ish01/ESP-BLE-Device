#pragma once
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>

BLEServer* BLECommon_createServer(const char* name);
