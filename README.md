# ESP32_BLE_Smart_HID_Device

A unified BLE library for ESP32 that acts both as a Human Interface Device (HID) and a Smart BLE device.

### Overview

This library allows your ESP32 to function in two dynamic modes:

## HID Mode – Acts as a Bluetooth Mouse.

Can handle clicks, scrolls and drags.



## Smart Mode – Acts as a data exchange device with a mobile app or BLE client.

Send and receive key:value BLE messages to control or sync data.

Useful for sending sensor readings (steps, heart rate, etc.) or control instructions (song control, camera click, etc.).

The modes can be switched on the fly, allowing your ESP32 to behave like a smart wearable or interactive input controller depending on context.


Implements a BLE UART-like service with TX (Notify) and RX (Write) characteristics.

Send structured messages (key:value) to or from a mobile device.



### Integration Ideas

Wearable gesture-based controller

BLE remote for phone camera / media / calls

Smart desktop controller (scroll, volume, app switch)

Data-sending IoT node with HID fallback



### Credits

HID report descriptor built using source logic from:

T-vK/BLE-Mouse

