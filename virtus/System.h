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


#define LEDC_BACKLIGHT_CHANNEL      3
#define LEDC_BACKLIGHT_BIT_WIDTH    8
#define LEDC_BACKLIGHT_FREQ         1000

class System {
public:
    static void resolve();

private:
    static void basic_boot();
    static void first_time_boot();
    static void os_init();
    static void active_loop();
};

extern System sys;

#endif