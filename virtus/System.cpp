#include "System.h"

// State Flags
static bool basic_boot_complete;
RTC_DATA_ATTR bool first_time_boot_complete; // zero-initialized on battery drain or restart
static bool force_complete_first_time_boot;
static bool os_init_complete;
static bool inactive;


void System::resolve() {
    es.println("entered resolve");
    while (true) {
        if (!basic_boot_complete) {
            basic_boot();
        } else if (!first_time_boot_complete && !force_complete_first_time_boot) {
            es.println("beginning first_time_boot stage");
            first_time_boot();
        } else if (!os_init_complete) {
            es.println("beginning os_init stage");
            os_init();
        } else {
            es.println("beginning active-Loop stage");
            active_loop();
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
    
    bios_mailbox = new Mailbox<int>();
    bios.setup_system_comm(bios_mailbox);
    work_manager.add_app_job(0); // first-time boot UI

    while (true) {        
        screen_manager.step();
        delay(2);
        
        int bios_result = -1;
        if (bios_mailbox->read(bios_result, 0)) {
            es.println("READ bios result!");
            es.println(bios_result);
            if (bios_result == 0) {
                first_time_boot_complete = true;
                break;
            }
            else if (bios_result == 1) {
                force_complete_first_time_boot = true;
                break;
            } 
        }
    }

    delete bios_mailbox;

}

void System::os_init() {
    os_init_complete = true;
}

void System::active_loop() {
    work_manager.add_app_job(1); // watch face
    while (true) {        
        screen_manager.step();
        delay(2);
    }
}