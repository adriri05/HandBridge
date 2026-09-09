# Gesture Protocol

Shared contract between hardware, web backend, and mobile app. Placeholder values — fill in as hardware is built.

## BLE service

| Item | Value |
|---|---|
| Service UUID | `TBD` |
| Sensor data characteristic UUID | `TBD` (notify) |
| Properties | Notify, ~20-50 Hz |

## Payload format

Proposed binary layout (little-endian), one BLE notification per reading:

| Bytes | Field | Type | Notes |
|---|---|---|---|
| 0-1 | `flexThumb` | uint16 | raw ADC or normalized 0-1023 |
| 2-3 | `flexIndex` | uint16 | raw ADC or normalized 0-1023 |
| 4-5 | `flexMiddle` | uint16 | raw ADC or normalized 0-1023 |
| 6-9 | `quatW` / or `roll` | float32 | IMU orientation, exact fields TBD (quaternion vs euler) |
| 10-13 | `quatX` / `pitch` | float32 | |
| 14-17 | `quatY` / `yaw` | float32 | |
| 18-21 | `quatZ` | float32 | omit if using euler angles |

Decoders in `mobile/` and `web/backend/` must stay in sync with this layout.

## Gesture mapping table

Not yet defined. To be filled in once hardware calibration is done — maps combinations of (flexThumb, flexIndex, flexMiddle, orientation) to output letters/words/commands.

| Gesture | Thumb | Index | Middle | Orientation | Output |
|---|---|---|---|---|---|
| TBD | | | | | |
