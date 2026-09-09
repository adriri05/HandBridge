# Architecture

## Components

- **Glove** — ESP32 microcontroller, 3x flex sensors (thumb, index, middle) on analog pins, IMU (MPU6050/BNO055, TBD) on I2C for hand orientation/motion. Firmware samples sensors, packages a reading, sends it as a BLE GATT notification.
- **Mobile app** — React Native, connects directly to glove over BLE (`react-native-ble-plx`), runs gesture decoding locally, displays text, speaks it via TTS. Primary/offline-capable client.
- **Web app** — React frontend talks to a Node/Express backend over WebSocket for live gesture stream + REST for history/config. Backend can either receive data relayed from the mobile app, or connect directly via Web Bluetooth from the browser (browser support permitting).
- **Gesture engine** — shared logic (conceptually; may start duplicated in mobile + backend until extracted to a shared package): maps a sensor reading (3 flex values + IMU orientation) to a letter, word, or command using the mapping table in [gesture-protocol.md](gesture-protocol.md).

## Data flow

1. ESP32 samples flex sensors + IMU at a fixed rate (e.g. 20-50 Hz).
2. Packs reading into the payload format defined in `gesture-protocol.md`.
3. Sends as BLE notification on the custom GATT characteristic.
4. Receiving client (mobile or web backend) buffers readings, applies gesture recognition (debounce + threshold matching against the mapping table).
5. Recognized letter/word appended to output text; TTS fires on word/sentence boundary.

## Open questions / TBD

- Exact IMU model (MPU6050 vs BNO055) — affects whether orientation fusion happens on-device or on-client.
- Gesture mapping table (which flex+orientation combos = which letters) — needs to be designed and put in `gesture-protocol.md`.
- Whether gesture engine logic gets extracted into a shared JS package consumed by both `mobile/` and `web/backend/`.
