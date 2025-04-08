#include <Arduino.h>
#include "System.h"

// State Flags
static bool basic_boot_complete;
RTC_DATA_ATTR bool first_time_boot_complete; // zero-initialized on battery drain or restart
static bool os_init_complete;
static bool inactive;

Battery battery;
Screen screen;
System sys;

System::System()
{
}

System::~System()
{
}

void System::resolve() {
    if (!basic_boot_complete) {
        basic_boot();
    }
}

void System::basic_boot() {
    btStop();
    setCpuFrequencyMhz(160);
    Serial.begin(115200);
    pinMode(BOARD_TOUCH_INT, INPUT);
    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
    Wire1.begin(BOARD_TOUCH_SDA, BOARD_TOUCH_SCL);

    battery.handle_basic_boot();
    screen.handle_basic_boot();

    delay(1000);

}