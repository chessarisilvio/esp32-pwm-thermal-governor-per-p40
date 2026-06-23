# Hardware Schematic

## Overview
This document describes the hardware connections between an ESP32 microcontroller, a DS18B20 temperature sensor, and the PWM-controlled fans of the NVIDIA Tesla P40 GPU. The ESP32 reads the GPU temperature via the sensor and adjusts fan speed using PWM signals driven through MOSFETs for safe current handling.

## Components
- **ESP32 DevKit** (e.g., ESP32-WROOM-32)
- **Temperature Sensor**: DS18B20 (waterproof version recommended)
- **Fan**: NVIDIA Tesla P40 stock fan (12V, PWM input)
- **MOSFET**: Logic-level N-channel MOSFET (e.g., IRLZ44N or equivalent) for PWM switching
- **Flyback Diode**: 1N4001 or similar (across fan terminals)
- **Pull-up Resistor**: 4.7 kΩ for DS18B20 data line
- **Gate Resistor**: 220 Ω (between ESP32 GPIO and MOSFET gate)
- **Power Supply**: 12V DC (for fan) and 3.3V (for ESP32 and sensor) – can be derived from a single 12V source with a buck converter or use separate supplies with common ground.

## Pin Assignment
| ESP32 GPIO | Function                     | Connection Details                                                                 |
|------------|------------------------------|-----------------------------------------------------------------------------------|
| GPIO4      | DS18B20 Data                 | Connected to DS18B20 data line via 4.7 kΩ pull‑up to 3.3V                         |
| GPIO18     | PWM Output (Fan Control)     | Connected to MOSFET gate through a 220 Ω resistor                                 |
| GND        | Common Ground                | Shared with DS18B20, MOSFET source, fan negative, and power supply grounds        |
| 3.3V       | Sensor Power                 | Powers DS18B20 VDD                                                                |
| Vin        | Optional 5V/12V Input        | If using onboard regulator; otherwise power ESP32 via USB or separate 3.3V regulator |

## Wiring Details

### DS18B20 Temperature Sensor
- **VDD** → ESP32 3.3V
- **GND** → ESP32 GND (common ground)
- **DATA** → ESP32 GPIO4, with a 4.7 kΩ pull‑up resistor to 3.3V

### MOSFET Driver Stage (Low‑Side Switch)
- **Gate** → ESP32 GPIO18 via 220 Ω resistor
- **Source** → ESP32 GND (common ground)
- **Drain** → Fan negative (black wire)
- **Fan positive (red wire)** → 12V supply V+
- **Flyback Diode** → Cathode to fan positive (12V), Anode to fan negative (MOSFET drain)

### Power Supply
- Use a stable 12V DC supply capable of delivering the fan’s rated current (check P40 fan spec, typically <0.5A).
- For the ESP32, either:
  - Power via USB (5V) with onboard regulator to 3.3V, **or**
  - Use a buck converter (12V → 3.3V) to power ESP32 and sensor.
- **Important**: All grounds (ESP32 GND, MOSFET source, fan negative, supply negative) must be tied together.

## Protection & Safety
- **Flyback Diode**: Prevents voltage spikes when the MOSFET turns off the inductive fan load.
- **Gate Resistor**: Dampens ringing and limits inrush current into the MOSFET gate capacitance.
- **Pull‑up Resistor**: Ensures reliable DS18B20 communication.
- **Current Rating**: Verify MOSFET can handle fan stall current; add a heatsink if necessary.
- **Decoupling**: Place a 0.1 µF ceramic capacitor close to ESP32 power pins.

## Optional Enhancements
- Add a second temperature sensor for redundancy.
- Use a logic‑level MOSFET with low Rds(on) to minimize voltage drop.
- Include a potentiometer or PWM frequency adjustment via software for fine‑tuning.
- Mount the sensor in thermal contact with the P40 heatsink (thermal paste or adhesive) for accurate readings.

## Diagram (ASCII)
```
+12V ----+-------------------+-------------------+
         |                   |
        [Fan]               [Diode] 1N4001
         |                   |   (Cathode to +12V)
         |                   |
         +------+------------+------+
                |             |
               [MOSFET]      |
                |             |
        GPIO18--[220Ω]------[Gate]
                |             |
               [GND]----------+------+------+------+
                               |      |      |      |
                        ESP32 GND  |      |      |
                                   |      |      |
                        GPIO4 ----[4.7kΩ]--+      |
                                           |      |
                                      [DS18B20]  |
                                           |      |
                                          GND------+
```
*Note: The diode is placed across the fan terminals (cathode to +12V, anode to MOSFET drain).*

## Bill of Materials (BOM)
| Item                     | Quantity | Notes                                      |
|--------------------------|----------|--------------------------------------------|
| ESP32 DevKit             | 1        | Any ESP32 with GPIO4 and GPIO18 available  |
| DS18B20 (waterproof)     | 1        |                                            |
| Logic‑Level N‑MOSFET     | 1        | IRLZ44N, AO3400A, or equivalent            |
| Flyback Diode (1N4001)   | 1        |                                            |
| Resistor 4.7 kΩ          | 1        | Pull‑up for DS18B20                        |
| Resistor 220 Ω           | 1        | MOSFET gate                                |
| 0.1 µF Ceramic Capacitor | 1        | Decoupling (optional)                      |
| Wires, Connectors, PCB   | –        | As needed                                  |
| Heatsink for MOSFET      | 1        | If dissipating >0.5W                       |

## Software Notes
- Use the OneWire library to read the DS18B20 on GPIO4.
- Configure LEDC PWM on GPIO18 (frequency 25 Hz typical for fans).
- Implement a PID or hysteresis controller to set fan duty cycle based on temperature thresholds.

---
*This schematic is intended for educational and hobbyist use. Double‑check all connections and ratings before powering the system.*