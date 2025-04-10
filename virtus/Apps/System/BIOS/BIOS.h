#ifndef BIOS_H
#define BIOS_H

#include "Apps/BaseApp.h"

class BIOS : public BaseApp {
public:
    virtual int mount(lv_obj_t* canvas);
    virtual int unmount(lv_obj_t* canvas);
};

extern BIOS first_time_boot_menu;

#endif