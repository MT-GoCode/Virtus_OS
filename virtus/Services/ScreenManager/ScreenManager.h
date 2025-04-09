#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "Drivers/Screen/Screen.h"

typedef enum {
    OFF, // screen not invoked yet
    BASIC_BOOT, // showing the logo on first-time boots or resets
    IDLE, // when there are CPU jobs running but no apps runing
    HOME, // home screen
    APP, // showing a particular app - either during first time boot showing the time sync options or later when showing one app.
} screen_state;

class ScreenManager : public Screen {
    friend class System;
    friend class WorkManager;
public:
protected:
    virtual int handle_basic_boot(bool first_time_boot_complete);
    // virtual int handle_os_init() { return 0; }
    // virtual int handle_active_loop() { return 0; }

    screen_state state = OFF;

    int mount_app(int id);
    void step();
};

extern ScreenManager screen_manager;

#endif