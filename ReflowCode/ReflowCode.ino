#include "max6675.h"

// --- Pin Definitions ---
int thermoDO = 12;
int thermoCS = 10;
int thermoCLK = 13;
int relayPin = 3; 

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// --- Reflow Profile Parameters (Celsius) ---
const float PREHEAT_TARGET = 150.0;
const float SOAK_TARGET    = 180.0;
const float REFLOW_PEAK    = 245.0;
const float COOL_DOWN_TEMP = 50.0;

const unsigned long SOAK_DURATION   = 90000;  // 90 seconds
const unsigned long REFLOW_DURATION = 45000;  // 45 seconds

// --- State Machine ---
enum State { IDLE, PREHEAT, SOAK, REFLOW, COOLING };
State currentState = IDLE;
unsigned long stateStartTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); 
  delay(500); 
  Serial.println("System Ready. Type 'S' in Serial Monitor to Start.");
}

void loop() {
  // Read Temperature in Celsius (Standard for Reflow)
  float currentC = thermocouple.readCelsius();

  // Basic Error Check
  if (isnan(currentC)) {
    digitalWrite(relayPin, LOW);
    currentState = IDLE;
    Serial.println("ERROR: Check Thermocouple!");
    delay(2000);
    return;
  }

  // Handle Serial Input to Start
  if (Serial.available() > 0) {
    char incoming = Serial.read();
    if (incoming == 'S' || incoming == 's') {
      if (currentState == IDLE) {
        currentState = PREHEAT;
        stateStartTime = millis();
        Serial.println("--- STARTING REFLOW ---");
      }
    }
  }

  updateStateMachine(currentC);
  controlHeater(currentC);

  // Status Output
  printStatus(currentC);

  delay(1000); 
}

void updateStateMachine(float currentTemp) {
  unsigned long elapsed = millis() - stateStartTime;

  switch (currentState) {
    case PREHEAT:
      if (currentTemp >= PREHEAT_TARGET) {
        currentState = SOAK;
        stateStartTime = millis();
        Serial.println("Entering SOAK phase...");
      }
      break;

    case SOAK:
      if (elapsed >= SOAK_DURATION) {
        currentState = REFLOW;
        stateStartTime = millis();
        Serial.println("Entering REFLOW (Peak) phase...");
      }
      break;

    case REFLOW:
      // Transition to cooling if time is up OR we hit peak safety
      if (elapsed >= REFLOW_DURATION || currentTemp >= REFLOW_PEAK) {
        currentState = COOLING;
        Serial.println("Entering COOLING phase...");
      }
      break;

    case COOLING:
      if (currentTemp <= COOL_DOWN_TEMP) {
        currentState = IDLE;
        Serial.println("--- REFLOW COMPLETE ---");
      }
      break;

    case IDLE:
      // Stay here until 'S' is pressed
      break;
  }
}

void controlHeater(float currentTemp) {
  // Safety: Heater always OFF in IDLE or COOLING
  if (currentState == IDLE || currentState == COOLING) {
    digitalWrite(relayPin, LOW);
    return;
  }

  // Simple On/Off Logic based on target for current state
  float target = 0;
  if (currentState == PREHEAT) target = PREHEAT_TARGET;
  if (currentState == SOAK)    target = SOAK_TARGET;
  if (currentState == REFLOW)  target = REFLOW_PEAK;

  if (currentTemp < target) {
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }
}

void printStatus(float temp) {
  Serial.print("Temp: "); Serial.print(temp); Serial.print(" C | State: ");
 switch (currentState) {
    case IDLE:    Serial.println("Status: IDLE (Waiting)"); break;
    case PREHEAT: Serial.print("Status: PREHEAT to "); Serial.print(PREHEAT_TARGET); Serial.println("C"); break;
    case SOAK:    Serial.print("Status: SOAK to ");    Serial.print(SOAK_TARGET);    Serial.println("C"); break;
    case REFLOW:  Serial.print("Status: REFLOW to ");  Serial.print(REFLOW_PEAK);    Serial.println("C"); break;
    case COOLING: Serial.print("Status: COOLING to "); Serial.print(COOL_DOWN_TEMP); Serial.println("C"); break;
  }
}
