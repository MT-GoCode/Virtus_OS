#ifndef BASEAPP_H
#define BASEAPP_H

#include <lvgl.h>

class BaseApp {
public:
    virtual ~BaseApp() {};

    virtual int mount(lv_obj_t* canvas);
    virtual int unmount();

    virtual void update_live_state() {};
};

#endif