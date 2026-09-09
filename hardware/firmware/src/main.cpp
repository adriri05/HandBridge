// HandBridge glove firmware — skeleton
// Reads 3 flex sensors + IMU, sends readings over BLE.
// Payload layout: see docs/gesture-protocol.md

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

// TODO: replace with real UUIDs, keep in sync with docs/gesture-protocol.md
#define SERVICE_UUID        "0000TBD0-0000-1000-8000-00805f9b34fb"
#define SENSOR_CHAR_UUID     "0000TBD1-0000-1000-8000-00805f9b34fb"

const int PIN_FLEX_THUMB = 32;
const int PIN_FLEX_INDEX = 33;
const int PIN_FLEX_MIDDLE = 34;

BLECharacteristic *sensorCharacteristic;

struct SensorReading {
  uint16_t flexThumb;
  uint16_t flexIndex;
  uint16_t flexMiddle;
  float roll;
  float pitch;
  float yaw;
};

SensorReading readSensors() {
  SensorReading r;
  r.flexThumb = analogRead(PIN_FLEX_THUMB);
  r.flexIndex = analogRead(PIN_FLEX_INDEX);
  r.flexMiddle = analogRead(PIN_FLEX_MIDDLE);

  // TODO: read IMU, compute orientation. Placeholder zeros until IMU is wired up.
  r.roll = 0.0f;
  r.pitch = 0.0f;
  r.yaw = 0.0f;
  return r;
}

void setup() {
  Serial.begin(115200);

  BLEDevice::init("HandBridge Glove");
  BLEServer *server = BLEDevice::createServer();
  BLEService *service = server->createService(SERVICE_UUID);

  sensorCharacteristic = service->createCharacteristic(
      SENSOR_CHAR_UUID,
      BLECharacteristic::PROPERTY_NOTIFY);

  service->start();
  server->getAdvertising()->start();

  Serial.println("HandBridge glove BLE advertising started");
}

void loop() {
  SensorReading r = readSensors();

  uint8_t payload[22];
  memcpy(payload + 0, &r.flexThumb, 2);
  memcpy(payload + 2, &r.flexIndex, 2);
  memcpy(payload + 4, &r.flexMiddle, 2);
  memcpy(payload + 6, &r.roll, 4);
  memcpy(payload + 10, &r.pitch, 4);
  memcpy(payload + 14, &r.yaw, 4);

  sensorCharacteristic->setValue(payload, sizeof(payload));
  sensorCharacteristic->notify();

  delay(33); // ~30 Hz
}
