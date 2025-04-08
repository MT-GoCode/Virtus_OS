#ifndef SCREEN_H
#define SCREEN_H

#include <TFT_eSPI.h>
#include <TouchDrvFT6X36.hpp>

#include "BaseDriver.h"
#include "Battery.h"

#define LEDC_BACKLIGHT_CHANNEL      3
#define LEDC_BACKLIGHT_BIT_WIDTH    8
#define LEDC_BACKLIGHT_FREQ         1000

class System;

class Screen : BaseDriver, TFT_eSPI, TouchDrvFT6X36 {
public:
    friend class System;
    Screen();
protected:
    virtual int handle_basic_boot() override;
    // virtual int handle_first_time_boot() override;
    // virtual int handle_os_init() override;
    // virtual int handle_active_loop() override;
    void setBrightness(uint8_t level);

    uint8_t brightness;
};

extern Screen screen;


#endif