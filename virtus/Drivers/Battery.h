#ifndef BATTERY_H
#define BATTERY_H

#include "XPowersLib.h"

#include "BaseDriver.h"

class System;
class Screen;

class Battery : BaseDriver, XPowersAXP2101 {
public:
    friend class System;
    friend class Screen;
    Battery();
protected:
    virtual int handle_basic_boot() override;
    // virtual int handle_first_time_boot() override;
    // virtual int handle_os_init() override;
    // virtual int handle_active_loop() override;
};

extern Battery battery;

#endif