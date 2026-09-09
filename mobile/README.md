# HandBridge — Mobile

React Native app. Connects directly to the glove over BLE, decodes gestures locally, displays text and speaks it via TTS. Works standalone without the web backend.

## Stack

- React Native
- `react-native-ble-plx` for BLE
- TTS: `react-native-tts` (or platform-native equivalent, TBD)

## Setup

```
cd mobile
npm install
npx react-native run-android   # or run-ios
```

### BLE permissions

- **Android**: `BLUETOOTH_SCAN`, `BLUETOOTH_CONNECT` (API 31+), location permission for BLE scan on older Android versions. Add to `AndroidManifest.xml`.
- **iOS**: `NSBluetoothAlwaysUsageDescription` in `Info.plist`.

## TODO

- [ ] Scaffold actual React Native project (this is a placeholder `src/`, not yet `npx react-native init`'d)
- [ ] BLE scan/connect flow to glove per docs/gesture-protocol.md
- [ ] Gesture decoding logic
- [ ] Text display + TTS output
- [ ] Optional: relay decoded gestures to web backend over the network
