# SmartScaleIoT 🍳⚖️

> An IoT-enabled 1 kg kitchen scale using a strain-gauge load cell, HX711 ADC, and Arduino Uno R3, with real-time Firebase cloud logging, a 16×2 I2C LCD display, and an interactive recipe-guided web dashboard for precise ingredient measurement.

<br/>

![Arduino](https://img.shields.io/badge/Arduino-Uno_R3-00979D?style=flat-square&logo=arduino&logoColor=white)
![Firebase](https://img.shields.io/badge/Firebase-RTDB-FFCA28?style=flat-square&logo=firebase&logoColor=black)
![Python](https://img.shields.io/badge/Python-3.x-3776AB?style=flat-square&logo=python&logoColor=white)
![HTML](https://img.shields.io/badge/Frontend-HTML%2FCSS%2FJS-E34F26?style=flat-square&logo=html5&logoColor=white)
![Course](https://img.shields.io/badge/ENR206-Sensors_%26_Instruments-c4622d?style=flat-square)

---

## Overview

**SmartScaleIoT** is a complete IoT weighing system built for ENR206 — Sensors and Instruments at Ahmedabad University. The system bridges the physical and digital — a load cell measures weight with ±1g precision, an Arduino processes the signal, Firebase logs every reading to the cloud, and a web dashboard turns the scale into an interactive recipe guide that ticks off ingredients one by one as you weigh them.

---

## Features

- **Real-time weight measurement** — HX711 24-bit ADC reads the strain-gauge load cell at 500ms intervals
- **LCD feedback** — 16×2 I2C display shows live weight directly on the hardware
- **Cloud logging** — Python serial bridge pushes every reading to Firebase Realtime Database
- **Recipe web app** — interactive dashboard guides you through recipes ingredient by ingredient
- **Auto-tick** — ingredient automatically marked complete when scale hits target weight (±3g tolerance)
- **Live Firebase listener** — web dashboard updates in real-time with no page refresh
- **Session analytics** — min, max, average, stability indicator, and reading count tracked per session
- **CSV export** — download all readings from any session with one click
- **Simulation fallback** — dashboard runs in demo mode if Firebase is offline

---

## Repository Structure

```
SmartScaleIoT/
│
├── index.html          # Main landing page — recipes, project report, circuit diagram, team
├── recipe.html         # Recipe cooking page — live scale + step-by-step ingredient guide
├── scale_firebase.py   # Python serial bridge — reads Arduino → pushes to Firebase RTDB
├── scale_lcd.ino       # Arduino firmware — HX711 + I2C LCD + Serial output
└── README.md
```

---

## Hardware

| Component | Specification |
|---|---|
| Microcontroller | Arduino Uno R3 (ATmega328P, 16 MHz) |
| Load Cell | Strain-gauge, 1 kg capacity, 4-wire |
| ADC Amplifier | HX711, 24-bit resolution |
| Display | 16×2 LCD with I2C backpack (address 0x27) |
| Communication | USB Serial at 57600 baud |
| Platform | Breadboard + MDF base |

---

## Wiring

### Load Cell → HX711

| Load Cell Wire | HX711 Pin |
|---|---|
| Red | E+ |
| Black | E− |
| Green | A+ |
| White | A− |

### HX711 → Arduino Uno R3

| HX711 Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| DT | D4 |
| SCK | D5 |

### LCD (I2C) → Arduino Uno R3

| LCD Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

---

## Software Setup

### 1. Arduino Firmware (`scale_lcd.ino`)

**Libraries required** — install via Arduino IDE Library Manager:
- `HX711_ADC` by Olav Kallhovd
- `LiquidCrystal_I2C` by Frank de Brabander

**Key parameters:**
```cpp
const int HX711_dout = 4;       // Data pin
const int HX711_sck  = 5;       // Clock pin
float calibrationValue = 2165.0; // Calibrated for 1 kg cell
```

Upload `scale_lcd.ino` to your Arduino Uno R3. Open Serial Monitor at **57600 baud** to verify readings.

To tare (zero) the scale, send the character `t` via Serial Monitor.

---

### 2. Python Bridge (`scale_firebase.py`)

**Install dependencies:**
```bash
pip install pyserial firebase-admin
```

**Firebase setup:**
1. Go to [Firebase Console](https://console.firebase.google.com/) → your project → Project Settings → Service Accounts
2. Generate a new private key → download as `serviceAccountKey.json`
3. Place `serviceAccountKey.json` in the same folder as `scale_firebase.py`

**Edit the script** — change the serial port to match your system:
```python
ser = serial.Serial('COM3', 57600, timeout=1)  # Windows
# ser = serial.Serial('/dev/ttyUSB0', 57600)   # Linux
# ser = serial.Serial('/dev/cu.usbmodem...', 57600)  # Mac
```

**Run:**
```bash
python scale_firebase.py
```

The script reads lines like `Weight (g): 172.5` from Arduino and pushes structured JSON to Firebase under the `weight_data` node:

```json
{
  "weight_g": 172.5,
  "timestamp": 1712750400.0,
  "datetime": "2026-04-10 14:30:00"
}
```

---

### 3. Web Dashboard

No server needed — open `index.html` directly in any modern browser.

**Firebase credentials** are already configured in the HTML files pointing to the project's RTDB. If you fork this project and use your own Firebase:

Find this line in both `index.html` and `recipe.html` and update the URL:
```javascript
firebase.initializeApp({
  databaseURL: 'https://your-project-default-rtdb.firebaseio.com/'
});
```

**`index.html`** — Landing page with:
- Recipe gallery (5 recipes, carousel)
- Project motivation and system architecture
- Full technical report
- Circuit diagram (SVG)
- Syntax-highlighted Arduino code
- Team section

**`recipe.html`** — Cooking mode with:
- Live scale reading from Firebase (updates every ~500ms)
- Ingredient checklist — auto-ticks when target weight is reached
- Progress bar and difference indicator ("4.2g to go")
- Step-by-step cooking method
- Works with `?r=0` through `?r=4` to select recipes

---

## Calibration

The calibration value `2165.0` was determined empirically using a known 172g reference mass (a smartphone). To recalibrate for your specific load cell:

1. Open the `Calibration.ino` example from the HX711_ADC library
2. Follow the serial prompts with a known reference weight
3. Update `calibrationValue` in `scale_lcd.ino` and `scale_firebase.py`

---

## System Architecture

```
┌─────────────┐    4-wire     ┌──────────┐   D4/D5    ┌──────────────────┐
│  Load Cell  │ ────────────► │  HX711   │ ─────────► │  Arduino Uno R3  │
│   1 kg      │               │ 24-bit   │            │  ATmega328P      │
└─────────────┘               └──────────┘            └──────┬───────────┘
                                                             │ A4/A5 (I2C)
                                                             │ USB Serial
                                              ┌──────────────┤
                                              │              ▼
                                    ┌─────────┴──────┐  ┌──────────────┐
                                    │  Python Bridge │  │  16×2 LCD    │
                                    │  pyserial      │  │  I2C 0x27    │
                                    └────────┬───────┘  └──────────────┘
                                             │
                                             ▼
                                    ┌────────────────┐
                                    │ Firebase RTDB  │
                                    │ weight_data/   │
                                    └────────┬───────┘
                                             │ JS SDK (real-time)
                                             ▼
                                    ┌────────────────┐
                                    │  Web Dashboard │
                                    │  index.html    │
                                    │  recipe.html   │
                                    └────────────────┘
```

---

## Results

| Metric | Value |
|---|---|
| Measurement range | 0 – 1000 g |
| Resolution | ±1 g (post-calibration) |
| Update interval | 500 ms |
| Firebase latency | < 300 ms (local network) |
| Calibration factor | 2165.0 |
| Reference mass used | 172 g (smartphone) |
| Baud rate | 57600 |

---

## Team

| Name | Enrollment No. | Role |
|---|---|---|
| Parthiv Karangiya | AU2440016 | Hardware & Firmware Lead |
| Aadi Mehta | AU2440095 | Firebase & Data Pipeline |
| Chintan Thacker | AU2440215 | Circuit Design & Testing |
| Aarav Patel | AU2440283 | Web Dashboard & UI |

**Course:** ENR206 — Sensors and Instruments
**Institution:** Ahmedabad University, 2024

---

## License

This project was developed as part of coursework at Ahmedabad University. Feel free to use and adapt it for educational purposes.

---

*Built with a strain gauge, some jumper wires, and a lot of calibration.*