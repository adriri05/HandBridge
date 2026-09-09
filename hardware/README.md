# HandBridge — Hardware

ESP32-based glove firmware: reads 3 flex sensors + an IMU, streams readings over BLE.

## Bill of materials (draft)

| Part | Qty | Notes |
|---|---|---|
| ESP32 dev board | 1 | BLE + ADC, e.g. ESP32-WROOM-32 |
| Flex sensor | 3 | thumb, index, middle finger |
| IMU | 1 | MPU6050 or BNO055 (TBD), mounted on back of hand, I2C |
| Resistor (for flex sensor voltage divider) | 3 | value depends on sensor spec sheet |
| LiPo battery + charger module | 1 | for wearable power |
| Glove (base garment) | 1 | sensors sewn/mounted on |

## Wiring (draft)

- Flex sensors: each wired as a voltage divider into an ESP32 ADC pin (e.g. GPIO 32, 33, 34).
- IMU: I2C (SDA/SCL, e.g. GPIO 21/22).
- Pinout to be finalized once IMU model is chosen — update this table.

| Signal | ESP32 pin |
|---|---|
| Flex — thumb | GPIO 32 (TBD) |
| Flex — index | GPIO 33 (TBD) |
| Flex — middle | GPIO 34 (TBD) |
| IMU SDA | GPIO 21 |
| IMU SCL | GPIO 22 |

## Firmware

PlatformIO project in `firmware/`. See `platformio.ini` for board config, `firmware/src/main.cpp` for the skeleton (sensor read + BLE notify).

### Build

```
cd hardware
pio run                 # build
pio run -t upload       # flash to ESP32
pio device monitor       # serial monitor
```

## TODO

- [ ] Pick IMU model, update pinout + BOM
- [ ] Implement BLE GATT service per `docs/gesture-protocol.md`
- [ ] Calibrate flex sensor ADC ranges
- [ ] Add IMU sensor fusion (orientation output)
- [ ] Power management (battery level reporting, sleep modes)
