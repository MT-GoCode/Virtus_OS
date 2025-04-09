#ifndef BATTERY_H
#define BATTERY_H

#include "XPowersLib.h"

#include "Drivers/BaseDriver.h"

class Battery : BaseDriver, public XPowersAXP2101 {
    friend class System;
    friend class Screen;
public:
protected:
    virtual int handle_basic_boot() override;
    // virtual int handle_first_time_boot() override;
    // virtual int handle_os_init() override;
    // virtual int handle_active_loop() override;
};

extern Battery battery;

#endif