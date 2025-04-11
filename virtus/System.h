#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

#include <FFat.h>
#include <FS.h>
#include <Wire.h>
#include <SensorDRV2605.hpp>
#include <driver/i2s.h>
#include <driver/temp_sensor.h>

#include "utilities.h"
#include "EasySerial.h"

#include "Drivers/Battery/Battery.h"
#include "Drivers/Screen/Screen.h"

#include "Services/ScreenManager/ScreenManager.h"
#include "Services/WorkManager/WorkManager.h"
#include "Services/TimeService/TimeService.h"
#include "Services/Mailbox/Mailbox.h"

#include "Apps/System/BIOS/BIOS.h"


#define LEDC_BACKLIGHT_CHANNEL      3
#define LEDC_BACKLIGHT_BIT_WIDTH    8
#define LEDC_BACKLIGHT_FREQ         1000

class System {
public:
    void resolve();

private:
    void basic_boot();
    void first_time_boot();
    void os_init();
    void active_loop();

    Mailbox<int>* bios_mailbox;
};

extern System sys;

#endif