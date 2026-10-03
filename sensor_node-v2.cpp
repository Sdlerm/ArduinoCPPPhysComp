// ============================================================================
// File: sensor_node-v2.cpp (Arduino Uno / Microcontroller Firmware)
// Target: Arduino Uno / ESP32 / Pi Pico
// Description: Non-blocking ADC sampling for Potentiometer / Sensor Voltage
//              Dividers, EMA filtering, bitwise debouncing, and CSV streaming.
// ============================================================================

#if __has_include(<Arduino.h>)
#include <Arduino.h>
#else
#include <cstdint>

#define HIGH 1
#define LOW 0
#define INPUT 0
#define INPUT_PULLUP 2
#define A0 14

class SerialClass {
public:
    void begin(unsigned long) {}
    void print(const char* value) { (void)value; }
    void print(char value) { (void)value; }
    void print(int value) { (void)value; }
    void print(unsigned int value) { (void)value; }
    void print(long value) { (void)value; }
    void print(unsigned long value) { (void)value; }
    void print(float value, int = 2) { (void)value; }
    void print(double value, int = 2) { (void)value; }
    void println(const char* value) { (void)value; }
    void println(char value) { (void)value; }
    void println(int value) { (void)value; }
    void println(unsigned int value) { (void)value; }
    void println(long value) { (void)value; }
    void println(unsigned long value) { (void)value; }
    void println(float value, int = 2) { (void)value; }
    void println(double value, int = 2) { (void)value; }
};

inline void pinMode(int, int) {}
inline int digitalRead(int) { return LOW; }
inline int analogRead(int) { (void)int(); return 0; }
inline unsigned long millis() { return 0; }

SerialClass Serial;
#endif

// --- Configuration ---
const int ADC_PIN = A0;           // Potentiometer / Voltage Divider Analog Pin
const int BTN_PIN = 2;            // Digital Button Input Pin (active-LOW)
const char* SENSOR_ID = "UNO_POT_ADC"; // Unique ID for SQLite Database

// Non-blocking Timing (50 Hz sampling)
unsigned long lastSampleTime = 0;
const unsigned long SAMPLE_INTERVAL_MS = 20;

// EMA Signal Filtering
float emaFiltered = 0.0;
const float ALPHA = 0.1; // 0.1 = smooth noise filtering

// Bitwise Shift-Register Debouncer
uint16_t debounceState = 0xFFFF;

void setup() {
    pinMode(ADC_PIN, INPUT);
    pinMode(BTN_PIN, INPUT_PULLUP);
    Serial.begin(115200);

    // Initialize EMA filter with baseline reading
    emaFiltered = analogRead(ADC_PIN);
}

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastSampleTime >= SAMPLE_INTERVAL_MS) {
        lastSampleTime = currentMillis;

        // 1. Read Potentiometer / Sensor Voltage Divider
        int rawAdc = analogRead(ADC_PIN);

        // 2. Exponential Moving Average (EMA) Noise Filter
        emaFiltered = (ALPHA * rawAdc) + ((1.0 - ALPHA) * emaFiltered);

        // 3. Convert to Millivolts (10-bit ADC, 5V reference for Arduino Uno)
        // For 3.3V / 12-bit boards (ESP32/Pico), change to: (emaFiltered * 3300.0) / 4095.0
        float voltageMv = (emaFiltered * 5000.0) / 1023.0;

        // 4. Bitwise Shift-Register Button Debounce
        debounceState = (debounceState << 1) | digitalRead(BTN_PIN) | 0xFE00;
        bool isButtonPressed = (debounceState == 0xFF00);

        // 5. Output CSV Payload over USB Serial
        // Format: SENSOR_ID,RAW,FILTERED,VOLTAGE_MV,BTN_EVENT
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
