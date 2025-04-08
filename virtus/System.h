#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

#include <FFat.h>
#include <FS.h>
#include <Wire.h>
#include <lvgl.h>
#include <SensorPCF8563.hpp>
#include <SensorDRV2605.hpp>
#include <driver/i2s.h>
#include <driver/temp_sensor.h>

#include "utilities.h"

#include "Drivers/Battery.h"
#include "Drivers/Screen.h"

#define LEDC_BACKLIGHT_CHANNEL      3
#define LEDC_BACKLIGHT_BIT_WIDTH    8
#define LEDC_BACKLIGHT_FREQ         1000

class System {
public:
    System();
    ~System();
    void resolve();

private:
    void basic_boot();
    void first_time_boot();
    void os_init();
    void active_loop();
};

extern System sys;

#endif