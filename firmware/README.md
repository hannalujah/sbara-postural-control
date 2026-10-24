# Firmware (SBARA robot)

`sbara/sbara.ino` is the Arduino Due firmware used to collect the data for this study on the SBARA balance
robot (Movafaghian Rehabilitation Center).

It reads four load cells through HX711 amplifiers (top-left, top-right, bottom-left, bottom-right), computes
the CoP, and drives the platform with PID position control from two encoders.

## Serial protocol

250000 baud. The PC sends a single-character command:

| Command | Action |
|---------|--------|
| `0` | return platform to home position |
| `1`–`3` | M/L disturbances (`Disturbance1`–`3`); the study uses `2` and `3` |
| `4`–`6` | A/P disturbances |
| `7`–`9` | further disturbance functions |

During a disturbance the robot streams one CoP sample per line:

```
<CoP_x>,<CoP_y>,<millis>
```

with CoP to one decimal place. This is the format stored in `data/processed/`.

## Building

* Board: Arduino Due
* Libraries (Arduino Library Manager): `Encoder`, `HX711`
