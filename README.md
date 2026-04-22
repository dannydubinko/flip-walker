# FlipWalker — PlatformIO Firmware

FlipWalker is an ESP32-based bipedal robot that locomotes by alternately flipping its two body segments forward. Joint angles are tracked with embedded magnets and MLX90393 magnetometers; two servo motors drive the flip gait while two DC motors provide lateral drive.

---

## Table of Contents

- [Hardware Overview](#hardware-overview)
- [Repository Structure](#repository-structure)
- [Firmware Variants](#firmware-variants)
  - [main.cpp — Plug-and-Play Serial (currently flashed)](#maincpp--plug-and-play-serial-currently-flashed)
  - [newMain.cpp — WiFi Web Interface](#newmaincpp--wifi-web-interface)
  - [Test / Utility Firmware](#test--utility-firmware)
- [Quick Start](#quick-start)
- [Configuration Reference (params.hpp)](#configuration-reference-paramshpp)
  - [Serial](#serial)
  - [I2C / Sensor Addresses](#i2c--sensor-addresses)
  - [Pin Assignments](#pin-assignments)
  - [Motor Parameters](#motor-parameters)
  - [WiFi (newMain.cpp only)](#wifi-newmaincpp-only)
  - [Magnetometer Sensing](#magnetometer-sensing)
  - [Calibration LUT Values](#calibration-lut-values)
- [Gait State Machine](#gait-state-machine)
- [Flip Cycle Timing](#flip-cycle-timing)
- [Serial Command Reference](#serial-command-reference)
- [Web API Reference (newMain.cpp)](#web-api-reference-newmaincpp)
- [Calibration Workflow](#calibration-workflow)
- [Switching Firmware Variants](#switching-firmware-variants)
- [Dependencies](#dependencies)

---

## Hardware Overview

| Component                 | Part              | Interface                    |
| ------------------------- | ----------------- | ---------------------------- |
| Microcontroller           | ESP32 (esp32dev)  | —                            |
| Joint sensor 1 (LEG_1)    | Adafruit MLX90393 | I2C `0x18`                   |
| Joint sensor 2 (LEG_2)    | Adafruit MLX90393 | I2C `0x1A`                   |
| Parent body servo         | Standard servo    | GPIO 13                      |
| Child body servo          | Standard servo    | GPIO 12                      |
| Left DC motor (H-bridge)  | —                 | GPIO 27 (IN1), GPIO 14 (IN2) |
| Right DC motor (H-bridge) | —                 | GPIO 26 (IN1), GPIO 25 (IN2) |

The two magnetometers sit at the joint between the parent and child body segments. An embedded magnet triggers whichever sensor is on the same side as the child segment, allowing the firmware to determine which configuration the robot is currently in without any additional position sensors.

---

## Repository Structure

```
FlipWalker-platformIO/
├── src/
│   ├── main.cpp                  # Production firmware — serial control, no WiFi
│   ├── Magnetometer.cpp          # MLX90393 driver (calibration + heading)
│   ├── MagnetometerCalibration.cpp # Interactive 7-point calibration capture tool
│   └── Calibration.cpp           # Calibration entry point (currently commented out)
├── include/
│   ├── params.hpp                # All configurable parameters — edit this file
│   ├── Magnetometer.hpp          # Magnetometer class interface
│   ├── MagnetometerCalibrator.hpp # Calibrator class interface
│   ├── HeadingEMA.hpp            # Exponential moving average filter for headings
│   └── password.hpp              # WiFi credentials (gitignored)
├── test/
│   ├── newMain.cpp               # WiFi web server variant with HTML dashboard
│   ├── mainNoWifi.cpp            # Serial-only variant (functionally same as main.cpp)
│   ├── magTesting.cpp            # Continuous sensor magnitude/presence diagnostic
│   ├── generalTest.cpp           # Full hardware test (servos + motors + sensors)
│   ├── servoTest.cpp             # Isolated servo sweep test
│   ├── findMagSensorAddy.cpp     # I2C bus scanner for finding sensor addresses
│   ├── wifiTest.cpp              # WiFi connectivity test
│   └── oldMain.cpp               # Archived earlier firmware version
├── data/                         # HTML assets for web interface (not compiled into firmware)
├── lib/                          # Local libraries (empty — deps managed via platformio.ini)
└── platformio.ini                # Build configuration and library dependencies
```

---

## Firmware Variants

### main.cpp — Plug-and-Play Serial (currently flashed)

`src/main.cpp` is the production firmware. It requires no WiFi and is controlled entirely over USB serial at 115200 baud. On boot it initializes both magnetometers with the LUT calibration values from `params.hpp` and enters the command loop.

**Use this when:** you want the simplest reliable operation — connect USB, open a serial monitor, type commands.

### newMain.cpp — WiFi Web Interface

`test/newMain.cpp` connects to the configured WiFi network, starts an HTTP server on port 80, and serves a responsive HTML dashboard. You control the robot from a browser on the same network. All endpoints return JSON.

**Use this when:** you want wireless control or want to integrate with external software via HTTP.

To flash this variant see [Switching Firmware Variants](#switching-firmware-variants).

### Test / Utility Firmware

| File                         | Purpose                                                                                                                    |
| ---------------------------- | -------------------------------------------------------------------------------------------------------------------------- |
| `test/mainNoWifi.cpp`        | Identical to `main.cpp` — serial control without WiFi stack                                                                |
| `test/magTesting.cpp`        | Prints magnitude + presence for both sensors every 100 ms; useful for verifying magnet placement and calibration threshold |
| `test/generalTest.cpp`       | Full hardware checkout — test servos, motors, and raw sensor readings from a single serial interface                       |
| `test/servoTest.cpp`         | Minimal servo sweep test; send `0`–`180` over serial to position servo                                                     |
| `test/findMagSensorAddy.cpp` | Scans the I2C bus and prints all found addresses — use when troubleshooting sensor wiring                                  |
| `test/wifiTest.cpp`          | WiFi connectivity diagnostics                                                                                              |

---

## Quick Start

1. Install [PlatformIO](https://platformio.org/) (VS Code extension or CLI).
2. Clone the repo and open the project folder in VS Code / PlatformIO IDE.
3. Connect the ESP32 via USB. Verify the upload port in `platformio.ini` matches your system:
   ```ini
   upload_port = /dev/cu.usbserial-210   ; macOS — change as needed
   monitor_port = /dev/tty.usbserial-210
   ```
   On Windows this will be a `COMx` port; on Linux `/dev/ttyUSBx`.
4. Click **Upload** (or run `pio run --target upload`).
5. Open the serial monitor at **115200 baud**.
6. Type `state` and press Enter — the robot will report whether the child segment is behind or ahead of the parent.
7. Type `flip` to execute one full flip-forward cycle.

---

## Configuration Reference (`params.hpp`)

All parameters are in `include/params.hpp`. Edit this file to adapt the firmware to your hardware.

### Serial

```cpp
namespace Params {
    const int serialPort = 115200;  // baud rate for USB serial monitor
}
```

### I2C / Sensor Addresses

```cpp
const int magnetometerAddr1 = 0x18;  // LEG_1 sensor (child-behind-parent)
const int magnetometerAddr2 = 0x1A;  // LEG_2 sensor (child-ahead-parent)
const int magnetometerAddr3 = 0x19;  // Reserved / unused
```

The MLX90393 I2C address is set by its A0/A1 address pins. Default addresses are `0x18`–`0x1B`. Use `findMagSensorAddy.cpp` to scan your bus if the addresses differ.

### Pin Assignments

```cpp
namespace Pins {
    const int servoPin1 = 13;      // Parent body servo signal pin
    const int servoPin2 = 12;      // Child body servo signal pin

    const int kMotor1_In1 = 27;    // Left motor H-bridge IN1
    const int kMotor1_In2 = 14;    // Left motor H-bridge IN2
    const int kMotor2_In1 = 26;    // Right motor H-bridge IN1
    const int kMotor2_In2 = 25;    // Right motor H-bridge IN2
}
```

Servos use the ESP32Servo library and accept 0–180° angles; 90° is the neutral/home position.

### Motor Parameters

```cpp
namespace Motors {
    const int leftMotor   = 1;     // Motor ID assigned as left
    const int rightMotor  = 2;     // Motor ID assigned as right

    const int leftForward  =  1;   // Multiplier: +1 or -1 depending on motor mounting direction
    const int rightForward = -1;   // Flip this if the right motor drives backwards

    const int driveSpeed  = 180;   // Default PWM drive speed (0–255)
}
```

`leftForward` / `rightForward` account for motors mounted in opposite orientations. If one wheel drives in the wrong direction, flip the corresponding multiplier between `1` and `-1` — no rewiring needed.

`driveSpeed` is the PWM value sent to the H-bridge for forward, backward, and turning commands. Range is `0` (stopped) to `255` (full speed).

### WiFi (`newMain.cpp` only)

```cpp
namespace Wifi {
    const char* wifi_name     = "USER";       // SSID of your network
    const char* wifi_password = "PASSWORD";  // Network password
    const int   portNumber    = 80;              // HTTP server port
}
```

Update `wifi_name` and `wifi_password` before flashing `newMain.cpp`. After boot the IP address is printed over serial — navigate to `http://<ip>/` in a browser on the same network.

### Magnetometer Sensing

```cpp
namespace MagSensorValues {
    const float minMagnitude = 600.0;
}
```

| Parameter      | Type    | Default | Unit      | Description                                                                                                                                                                              |
| -------------- | ------- | ------- | --------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `minMagnitude` | `float` | `600.0` | arbitrary | Minimum magnetic field magnitude required for a sensor to report "magnet present". Increase if false positives occur; decrease if the magnet isn't being detected at the expected range. |

Sensor readings below this threshold cause `isMagnetPresent()` to return `false` and heading functions to return `NAN`.

**Sensor hardware settings** (set inside `Magnetometer::initialize()` in `Magnetometer.cpp`):

| Setting      | Value               | Effect                                |
| ------------ | ------------------- | ------------------------------------- |
| Gain         | `MLX90393_GAIN_1X`  | Full-range field sensitivity          |
| Resolution   | `MLX90393_RES_16`   | 16-bit ADC per axis                   |
| Oversampling | `MLX90393_OSR_3`    | 3× hardware averaging — reduces noise |
| Filter       | `MLX90393_FILTER_5` | Digital low-pass — further smoothing  |

### Calibration LUT Values

The magnetometer output (raw heading in degrees from `atan2(X, Z)`) varies with local magnetic field distortion and sensor mounting. A 7-point lookup table (LUT) maps raw sensor output to actual physical joint angles.

```cpp
namespace MagSensorValues {
    // Physical angles at which calibration samples were captured (degrees)
    // These are fixed reference positions — do not change unless you change the calibration procedure
    const float PHYSICAL_ANGLES[7] = {90.0, 60.0, 30.0, 0.0, -30.0, -60.0, -90.0};

    // Raw sensor output at each physical angle for LEG_1 (sensor at 0x18)
    const float RAW_READINGS_LEG_1[7] = {-12.3284, -12.3132, -8.6361, -4.3975, 0.6608, 4.4029, 6.3600};

    // Raw sensor output at each physical angle for LEG_2 (sensor at 0x1A)
    const float RAW_READINGS_LEG_2[7] = {-6.7369, -6.5691, -4.7230, -1.4843, 3.7297, 7.2333, 8.7587};

    // Reserved for a potential third sensor — currently unused
    const float RAW_READINGS_LEG_3[7] = {0, 0, 0, 0, 0, 0, 0};
}
```

These values are specific to the physical robot. If the magnets are repositioned or the sensors are remounted, recalibrate using the [Calibration Workflow](#calibration-workflow) and replace these arrays.

---

## Gait State Machine

The firmware tracks two states based on which magnetometer detects the embedded joint magnet:

| State     | Meaning                              | Active sensor                 |
| --------- | ------------------------------------ | ----------------------------- |
| `cBp`     | Child segment is **behind** parent   | Sensor 1 (`0x18`) sees magnet |
| `cAp`     | Child segment is **ahead of** parent | Sensor 2 (`0x1A`) sees magnet |
| `UNKNOWN` | Mid-flip transition or sensor error  | Neither / both sensors active |

`UNKNOWN` is a transient state during the flip cycle while the body segments are stacked vertically. A 5-second timeout prevents the robot from hanging in this state if a sensor fails.

---

## Flip Cycle Timing

Each `flip` command advances the robot one half-cycle forward. Two consecutive flips complete a full locomotion step.

### cBp → cAp (child flips forward)

| Step | Action                                                 | Delay   |
| ---- | ------------------------------------------------------ | ------- |
| 1    | Child servo → 0° (swing child forward)                 | 1500 ms |
| 2    | Wait for both sensors to clear (body stacked, max 5 s) | —       |
| 3    | Child servo → 90° (return to neutral)                  | 2000 ms |
| 4    | Parent servo → 20° (partial intermediate)              | 1500 ms |
| 5    | Parent servo → 90° (return to neutral)                 | 1500 ms |

### cAp → cBp (parent flips forward)

| Step | Action                                                 | Delay   |
| ---- | ------------------------------------------------------ | ------- |
| 1    | Parent servo → 180° (swing parent forward)             | 1500 ms |
| 2    | Wait for both sensors to clear (body stacked, max 5 s) | —       |
| 3    | Parent servo → 90° (return to neutral)                 | 2000 ms |
| 4    | Child servo → 160° (partial intermediate)              | 1500 ms |
| 5    | Child servo → 90° (return to neutral)                  | 1500 ms |

The intermediate positions (20° / 160°) during the return strokes help the non-driving segment clear the ground without a full swing. All timing values are hardcoded in `src/main.cpp` inside `doFlip()`.

---

## Serial Command Reference

Open a serial monitor at **115200 baud** and send these commands (press Enter after each):

| Command    | Description                                                               |
| ---------- | ------------------------------------------------------------------------- |
| `r`        | Read both sensors — prints raw heading and magnet-present status for each |
| `state`    | Print current robot state (`cBp`, `cAp`, or `UNKNOWN`)                    |
| `flip`     | Execute one flip-forward cycle from the current state                     |
| `p<angle>` | Move the **parent** servo to `angle` degrees (0–180). Example: `p45`      |
| `c<angle>` | Move the **child** servo to `angle` degrees (0–180). Example: `c130`      |

These commands are the same in both `main.cpp` and `mainNoWifi.cpp`.

---

## Web API Reference (`newMain.cpp`)

After flashing `newMain.cpp`, find the ESP32's IP address in the serial monitor and navigate to it in a browser. A dashboard with drive and flip buttons is served at `/`.

### Endpoints

#### `GET /`

Returns the HTML control dashboard.

#### `GET /cmd?v=<command>`

Executes a command and returns a JSON state object.

| Command value | Action                                                     |
| ------------- | ---------------------------------------------------------- |
| `fwd`         | Drive both motors forward at `driveSpeed`                  |
| `bwd`         | Drive both motors backward at `driveSpeed`                 |
| `left`        | Left motor stops, right motor drives forward (pivot left)  |
| `right`       | Right motor stops, left motor drives forward (pivot right) |
| `stop`        | Stop both motors                                           |
| `flip`        | Execute one flip-forward cycle                             |
| `p<angle>`    | Move parent servo (e.g. `p90`)                             |
| `c<angle>`    | Move child servo (e.g. `c45`)                              |

**Response example:**

```json
{
  "state": "cBp",
  "mag1": 1234.5,
  "mag1_present": true,
  "mag2": 876.3,
  "mag2_present": false
}
```

#### `GET /sensors`

Returns live sensor data without executing any command.

```json
{
  "mag1_mag": 1234.5,
  "mag1_present": true,
  "mag2_mag": 876.3,
  "mag2_present": false
}
```

---

## Calibration Workflow

Recalibrate if:

- The magnets or sensors are repositioned
- The robot is rebuilt with different hardware
- `getLinearHeading()` returns unreasonable angles

### Steps

1. **Enable Calibration.cpp** — in `src/Calibration.cpp`, uncomment the `setup()` / `loop()` entry points, then comment out `src/main.cpp` in `platformio.ini` so Calibration.cpp is the active firmware (see [Switching Firmware Variants](#switching-firmware-variants)).

2. **Flash and open serial monitor** at 115200 baud.

3. **Follow the interactive prompts** — for each of the 7 reference angles (`±90°`, `±60°`, `±30°`, `0°`) you will:
   - Manually position the joint to the target angle
   - Press Enter
   - The firmware captures 50 averaged samples

4. **Copy the output** — after all 7 points are captured, the calibrator prints ready-to-paste C++ arrays:

   ```
   float RAW_READINGS_LEG_1[7] = { ... };
   ```

5. **Paste into `params.hpp`** — replace the existing `RAW_READINGS_LEG_1` or `LEG_2` array with the new values.

6. **Restore `main.cpp`** as the active firmware and reflash.

### Calibration internals

- 50-sample average per angle position reduces noise
- All raw values are normalized relative to the `0°` reading to remove atan2 wrap-around discontinuities
- The LUT is sorted by raw value ascending to support bracket-search linear interpolation at runtime
- `getLinearHeading()` clamps output to ±90° and returns `NAN` if no magnet is detected

---

## Switching Firmware Variants

To flash a different firmware variant, edit `platformio.ini`:

```ini
[env:esp32dev]
platform  = espressif32
board     = esp32dev
framework = arduino

monitor_speed = 115200

; --- Uncomment exactly ONE of the src_filter lines below ---

; Production serial firmware (default — currently flashed)
; src_filter = +<src/main.cpp> +<src/Magnetometer.cpp> +<src/MagnetometerCalibration.cpp>

; WiFi web interface
; src_filter = +<test/newMain.cpp> +<src/Magnetometer.cpp> +<src/MagnetometerCalibration.cpp>

; Sensor diagnostic
; src_filter = +<test/magTesting.cpp> +<src/Magnetometer.cpp>

; Full hardware test
; src_filter = +<test/generalTest.cpp> +<src/Magnetometer.cpp>

; Calibration mode
; src_filter = +<src/Calibration.cpp> +<src/Magnetometer.cpp> +<src/MagnetometerCalibration.cpp>
```

> If `platformio.ini` does not already contain `src_filter` lines, add them manually. By default PlatformIO compiles everything in `src/` — using `src_filter` gives you explicit control over which entry point is active.

---

## Dependencies

Declared in `platformio.ini` under `lib_deps`:

| Library                      | Version     | Purpose                                                     |
| ---------------------------- | ----------- | ----------------------------------------------------------- |
| `madhephaestus/ESP32Servo`   | `^3.1.1`    | PWM servo control on ESP32                                  |
| `electroniccats/MPU6050`     | `^1.4.4`    | IMU driver (present, not actively used in primary firmware) |
| `adafruit/Adafruit_MLX90393` | GitHub HEAD | Magnetometer driver                                         |

PlatformIO downloads and caches these automatically on first build.
