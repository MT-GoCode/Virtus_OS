#include "System.h"

// State Flags
static bool basic_boot_complete;
RTC_DATA_ATTR bool first_time_boot_complete; // zero-initialized on battery drain or restart
static bool os_init_complete;
static bool inactive;

void System::resolve() {
    while (true) {
        if (!basic_boot_complete) {
            basic_boot();
        } else if (!first_time_boot_complete) {
            first_time_boot();
        } else {
            break;
        }
    }
}

void System::basic_boot() {
    es.println("starting...");
    btStop();
    setCpuFrequencyMhz(160);
    Serial.begin(115200);
    pinMode(BOARD_TOUCH_INT, INPUT);
    Wire.begin(BOARD_I2C_SDA, BOARD_I2C_SCL);
    Wire1.begin(BOARD_TOUCH_SDA, BOARD_TOUCH_SCL);

    battery.handle_basic_boot();
    screen.handle_basic_boot();
    screen_manager.handle_basic_boot(first_time_boot_complete);
    basic_boot_complete = true;
}

void System::first_time_boot() {
    es.println("done with timeservice handle first time boot");
    work_manager.add_app_job(0); // first-time boot UI

    while (true) {        
        screen_manager.step();
        delay(2);
        // purposely blocking forever -- just want to show the UI from hereon out.
    }
}