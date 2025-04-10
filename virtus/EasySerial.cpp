#include "EasySerial.h"

EasySerial es;

EasySerial::EasySerial(uint32_t baud_rate) : baud_rate_(baud_rate) {
    begin();
}

void EasySerial::begin() {
    Serial.begin(baud_rate_);
    while (!Serial) {
        ; // Wait for Serial to initialize (optional, useful for some boards)
    }
}

void EasySerial::printTimestamp() {
    unsigned long ms = millis();
    Serial.print("[");
    Serial.print(ms / 1000); // Seconds
    Serial.print(".");
    if (ms % 1000 < 100) Serial.print("0");
    if (ms % 1000 < 10) Serial.print("0");
    Serial.print(ms % 1000); // Milliseconds
    Serial.print("] ");
}

void EasySerial::print(const char* message) {
    printTimestamp();
    Serial.print(message);
}

void EasySerial::print(const String& message) {
    printTimestamp();
    Serial.print(message);
}

void EasySerial::print(int value) {
    printTimestamp();
    Serial.print(value);
}

void EasySerial::print(unsigned long value) {
    printTimestamp();
    Serial.print(value);
}

void EasySerial::println(const char* message) {
    printTimestamp();
    Serial.println(message);
}

void EasySerial::println(const String& message) {
    printTimestamp();
    Serial.println(message);
}

void EasySerial::println(int value) {
    printTimestamp();
    Serial.println(value);
}

void EasySerial::println(unsigned long value) {
    printTimestamp();
    Serial.println(value);
}
