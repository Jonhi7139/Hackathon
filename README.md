# Hackathon — Real-Time Sensor Telemetry Pipeline

A firmware-to-dashboard pipeline that reads a 13-DOF sensor stack from a microcontroller, streams the raw readings over serial as a packed binary struct, decodes them in Python, persists them to PostgreSQL, and visualizes them live in Grafana.

## How it works

1. **Firmware** (`Microcontroller/`) reads the accelerometer, gyroscope, magnetometer, and environmental sensor, packs every reading into a single fixed-layout `struct`, and writes it raw over serial, prefixed with a `##` header.
2. **Receiver** (`Receiver/`) listens on the serial port, waits for the `##` header, reads the exact number of bytes the struct occupies, and unpacks them with Python's `struct` module using a format string that mirrors the C++ struct layout byte-for-byte.
3. Each decoded reading is inserted into a PostgreSQL table and committed immediately, so data lands in the database in near real time.
4. **Grafana** queries that table directly and auto-refreshes, giving a live view of the last N readings for each sensor group.

## Hardware & sensors

The firmware targets an **Arduino Zero** and drives three I2C sensors combined into a single 13DOF board, using a custom library `B13DOF` to configure the sesnor:

| Sensor | Measures | Library |
|---|---|---|
| Bosch BMI088 | Acceleration (3-axis) + angular velocity (3-axis) + internal temp | `bolderflight/bmi088-arduino` |
| Bosch BME680 | External temperature, humidity, pressure, gas resistance | `adafruit/Adafruit BME680 Library` |
| Bosch BMM150 | Magnetic field (3-axis) | `DFRobot/DFRobot_BMM150` |

## Fowarding protocol

Every reading is sent as one packed, little-endian C struct (`environment`, in `B13DOF.h`):

```cpp
struct __attribute__((__packed__)) environment {
    float    ax, ay, az;   // Accelerometer (m/s²)
    float    gx, gy, gz;   // Gyroscope (rad/s)
    float    tempI;        // BMI088 internal temperature (°C)
    uint64_t time;         // Sensor timestamp (ps)
    float    tempO;        // BME680 external temperature (°C)
    float    humidity;     // BME680 humidity (%)
    float    pressure;     // BME680 pressure (hPa)
    uint32_t Gas;           // BME680 gas resistance (KΩ)
    int16_t  mx, my, mz;   // BMM150 magnetic field (µT)
};
```

Each frame on the wire looks like:

```
[ '#' ] [ '#' ] [ 58 bytes of packed struct data ] [ '\n' ]
```

- `Serializer::serialSend()` writes the `##` header, then the struct byte-by-byte via `reinterpret_cast<uint8_t*>`.
- On the Python side, `Receiver.store()` blocks until it sees `##`, reads exactly 58 bytes, and decodes them with:
  ```python
  struct.unpack("<fffffffQfffIhhh", raw_data)
  ```
  The format string matches the struct field-for-field: 7 floats, 1 unsigned 64-bit int, 3 floats, 1 unsigned 32-bit int, 3 signed 16-bit ints.

## Repository structure

```
Hackathon/
├── Microcontroller/
│   ├── platformio.ini        # Board, framework, and sensor library dependencies
│   └── src/
│       ├── B13DOF.h / .cpp   # Sensor init, config, and reading logic; defines `environment`
│       ├── Serializer.h/.cpp # Packs `environment` and writes it to Serial
│       └── main.cpp          # Setup + loop: reads sensors, sends framed packets
├── Receiver/
│   ├── Receiver.py           # Serial framing + struct unpacking
│   └── main.py                # Opens the serial port and DB connection, inserts each reading
├── Scheme.sql                 # PostgreSQL table definition (sensor_telemetry)
└── dashboard.json             # Grafana dashboard (JSON export, schema v2)
```

## Database

`Scheme.sql` creates a single table, `sensor_telemetry`, with one row per sensor reading plus an auto-incrementing `id` and a `created_at` timestamp defaulting to `CURRENT_TIMESTAMP`. Column names map directly onto the fields of the `environment` struct (`ax`, `ay`, `az`, `gx`, `gy`, `gz`, `temp_i`, `time_raw`, `temp_o`, `humidity`, `pressure`, `gas`, `mx`, `my`, `mz`).

## Grafana dashboard

`dashboard.json` is a Grafana v13 dashboard (schema `dashboard.grafana.app/v2`) with a PostgreSQL data source pointed at the `Dynamics` database, default time range `now-6h` to `now`, with four panels:

| Panel | Type | Shows |
|---|---|---|
| Gyroscope | Gauge | Latest accelerometer (X/Y/Z) and gyroscope (X/Y/Z) values |
| Temperature | Bar gauge | Internal (BMI088) vs. external (BME680) temperature |
| Volatiles | Bar gauge | Humidity, pressure, gas resistance |
| Magnet | Gauge | Magnetometer X/Y/Z |

Each panel queries the last 20 rows (`ORDER BY id DESC LIMIT 20`, re-sorted ascending) so the dashboard always reflects the most recent window of readings rather than the full history.

To import it: Grafana → Dashboards → New → Import → upload `dashboard.json`, then point the PostgreSQL data source at your local `Dynamics` database.

