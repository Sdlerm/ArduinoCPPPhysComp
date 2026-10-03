// ============================================================================
// File: sensor_node.cpp (Microcontroller Firmware)
// Target: Arduino Uno / ESP32 / Pi Pico (Arduino Framework)
// Description: Non-blocking ADC sampling, EMA filtering, bitwise debouncing, 
//              and CSV serial stream generation.
// ============================================================================

#include <Arduino.h>

// Pin Configuration
const int ADC_PIN = A0;        // Analog sensor input pin
const int BTN_PIN = 2;         // Digital button input pin (active-LOW, using pullup)
const char* SENSOR_ID = "ADC_NODE_01";

// Non-blocking Timing Configuration
unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_INTERVAL_MS = 20; // 50 Hz sampling rate

// Signal Conditioning (EMA Filter)
float emaFiltered = 0.0;
const float ALPHA = 0.1; // Smoothing factor (0.1 = heavy filtering, 0.5 = fast response)

// Bitwise Shift-Register Debouncer State
uint16_t debounceState = 0xFFFF;

void setup() {
    pinMode(ADC_PIN, INPUT);
    pinMode(BTN_PIN, INPUT_PULLUP);
    Serial.begin(115200);

    // Initialize EMA filter with initial reading
    emaFiltered = analogRead(ADC_PIN);
}

void loop() {
    unsigned long currentMillis = millis();

    // 1. Non-Blocking Sampler Loop
    if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = currentMillis;

        // --- Read Analog Sensor ---
        int rawAdc = analogRead(ADC_PIN);

        // --- Apply Exponential Moving Average (EMA) Filter ---
        emaFiltered = (ALPHA * rawAdc) + ((1.0 - ALPHA) * emaFiltered);

        // --- Convert to Voltage (mV) for 10-bit 5V ADC (Arduino Uno) ---
        // For 3.3V / 12-bit ADCs (ESP32/Pico), adjust multiplier to (3300.0 / 4095.0)
        float voltageMv = (emaFiltered * 5000.0) / 1023.0;

        // --- Bitwise Shift-Register Debounce ---
        // Shifts in current button state; 0xFF00 signals 8 consecutive stable LOW reads
        debounceState = (debounceState << 1) | digitalRead(BTN_PIN) | 0xFE00;
        bool isButtonPressed = (debounceState == 0xFF00);

        // --- Format CSV Payload ---
        // Payload format: SENSOR_ID,RAW,FILTERED,VOLTAGE_MV,BTN_EVENT
        Serial.print(SENSOR_ID);
        Serial.print(",");
        Serial.print(rawAdc);
        Serial.print(",");
        Serial.print(emaFiltered, 2);
        Serial.print(",");
        Serial.print(voltageMv, 2);
        Serial.print(",");
        Serial.println(isButtonPressed ? 1 : 0);
    }
}
