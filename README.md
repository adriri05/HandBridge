# HandBridge

Glove-based communication device. Flex sensors on thumb, index, and middle finger, plus an IMU on the back of the hand, translate finger and hand-motion gestures into text and speech.

## How it works

```
[Glove]                [Phone / Browser]              [Cloud/local]
 ESP32 + 3x flex   --BLE-->  Mobile App (RN)   
 sensors + IMU     --BLE-->  Web App (Web Bluetooth)  --WS-->  Backend
                                    |
                              Gesture Engine
                              (flex combo + IMU orientation -> letter/word)
                                    |
                              Text display + Text-to-Speech
```

1. **Glove (hardware/)** — ESP32 reads 3 flex sensor values + IMU orientation/motion, streams over BLE.
2. **Web app (web/)** — React frontend + Node/Express backend. Backend receives gesture data (via Web Bluetooth in-browser or relayed from mobile), decodes gestures, serves live view over WebSocket.
3. **Mobile app (mobile/)** — React Native app, connects to glove over BLE, decodes gestures, shows text, speaks via TTS. Works standalone without the web backend.

Shared data contract for all three: [docs/gesture-protocol.md](docs/gesture-protocol.md).

## Project structure

| Folder | Purpose |
|---|---|
| [hardware/](hardware/README.md) | Firmware (ESP32/PlatformIO), wiring, BOM |
| [web/](web/README.md) | React frontend + Express backend |
| [mobile/](mobile/README.md) | React Native app |
| [docs/](docs/architecture.md) | Architecture + gesture protocol spec |

## Status

Early scaffold. Structure in place, implementations are placeholders. See each subfolder's README for TODOs.
