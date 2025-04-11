#ifndef DEFAULTWATCHFACE_H
#define DEFAULTWATCHFACE_H

#include "Apps/BaseApp.h"

class DefaultWatchFace : public BaseApp {
public:
    virtual int mount(lv_obj_t* canvas);
    virtual int unmount();
    virtual void update_live_state();

private:
    lv_obj_t* frame;
    lv_obj_t* full_time_container;
    lv_obj_t* hh_mm_label;
    lv_obj_t* am_pm_label;
    lv_obj_t* day_date_container;
    lv_obj_t* day_label;
    lv_obj_t* date_label;
};

extern DefaultWatchFace default_watch_face;

#endif