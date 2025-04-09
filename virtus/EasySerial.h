#ifndef SERIAL_DEBUGGER_H
#define SERIAL_DEBUGGER_H

#include <Arduino.h>

class EasySerial {
public:
    // Constructor: Initialize with a baud rate
    EasySerial(uint32_t baud_rate = 115200);

    // Initialize the Serial port
    void begin();

    // Print with timestamp (no newline)
    void print(const char* message);
    void print(const String& message);
    void print(int value);
    void print(unsigned long value);

    // Print with timestamp and newline
    void println(const char* message);
    void println(const String& message);
    void println(int value);
    void println(unsigned long value);

private:
    uint32_t baud_rate_; // Baud rate for Serial communication
    void printTimestamp(); // Helper to print the timestamp
};

extern EasySerial es;

#endif // SERIAL_DEBUGGER_H