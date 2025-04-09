#ifndef FIRSTTIMEBOOTMENU_H
#define FIRSTTIMEBOOTMENU_H

#include "Apps/BaseApp.h"

class FirstTimeBootMenu : public BaseApp {
public:
    virtual int mount(lv_obj_t* canvas);
    virtual int unmount(lv_obj_t* canvas);
};

extern FirstTimeBootMenu first_time_boot_menu;

#endif