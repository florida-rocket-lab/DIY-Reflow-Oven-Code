# Arduino Reflow Oven Controller

This project implements a simple **reflow oven controller** using an Arduino, a **MAX6675 thermocouple amplifier**, and a relay to control a heating element. It follows a standard **reflow soldering temperature profile** using a state machine.

---

## 📌 Features

* Reads temperature using a MAX6675 thermocouple
* Controls a heating element via relay
* Implements a multi-stage reflow profile:

  * Preheat
  * Soak
  * Reflow (Peak)
  * Cooling
 
* Serial-based control and monitoring
* Basic safety handling (sensor failure + temperature limits)

---

## Hardware Requirements

* Arduino (Uno, Nano, etc.)
* MAX6675 Thermocouple Module
* K-Type Thermocouple
* Relay module (or SSR recommended for safety)
* Heating element (toaster oven, hot plate, etc.)
* Power supply

---

## Pin Configuration

| Component       | Arduino Pin |
| --------------- | ----------- |
| MAX6675 SO (DO) | 12          |
| MAX6675 CS      | 10          |
| MAX6675 SCK     | 13          |
| Relay Control   | 3           |

---

## Reflow Profile

| Phase   | Target Temp (°C) | Duration       |
| ------- | ---------------- | -------------- |
| Preheat | 150              | Until reached  |
| Soak    | 180              | 90 seconds     |
| Reflow  | 245 (peak)       | 45 seconds max |
| Cooling | 50               | Until reached  |

---

##  How It Works

### 1. Start the Process

* Open the **Serial Monitor**
* Set baud rate to **9600**
* Send:

  ```
  S
  ```
* The system begins the reflow cycle

---

### 2. State Machine

The controller uses a state machine with the following states:

* `IDLE` → Waiting for user input
* `PREHEAT` → Heat up to 150°C
* `SOAK` → Hold around 180°C for 90 seconds
* `REFLOW` → Ramp to peak temperature (245°C)
* `COOLING` → Turn off heater and cool down

---

### 3. Heater Control Logic

* Heater turns **ON** when temperature is below target
* Heater turns **OFF** when target is reached
* Heater is always OFF during:

  * `IDLE`
  * `COOLING`

---

## Serial Output Example

```
Temp: 135.00 C | Status: PREHEAT to 150C
Temp: 152.00 C | Entering SOAK phase...
Temp: 180.00 C | Status: SOAK to 180C
Temp: 230.00 C | Entering REFLOW phase...
Temp: 245.00 C | Entering COOLING phase...
--- REFLOW COMPLETE ---
```

---

## Possible Improvements

* PID control instead of simple on/off
* LCD or OLED display
* Rotary encoder or buttons for control
* SD card logging
* Multiple thermocouples for accuracy
* Graphing temperature curve via Serial Plotter
