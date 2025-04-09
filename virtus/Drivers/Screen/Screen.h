#ifndef SCREEN_H
#define SCREEN_H

#include <TFT_eSPI.h>
#include <TouchDrvFT6X36.hpp>
#include <lvgl.h>

#include "Drivers/BaseDriver.h"
#include "Drivers/Battery/Battery.h"
#include "utilities.h"

#define LEDC_BACKLIGHT_CHANNEL      3
#define LEDC_BACKLIGHT_BIT_WIDTH    8
#define LEDC_BACKLIGHT_FREQ         1000

class Screen : BaseDriver, public TFT_eSPI, public TouchDrvFT6X36 {
    friend class System;

public:
    void begin_lvgl(bool debug = false);
    void update_lvgl();

protected:
    virtual int handle_basic_boot() override;
    void setBrightness(uint8_t level);

    uint8_t brightness;

private:
    static void disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p);
    static void touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data);

    static Screen* instance;
    static lv_disp_drv_t disp_drv;
    static lv_indev_drv_t indev_drv;
    static lv_disp_draw_buf_t draw_buf;

#ifdef BOARD_HAS_PSRAM
    static lv_color_t* buf;
    static lv_color_t* buf1;
#else
    static lv_color_t buf[];
    static lv_color_t buf1[];
#endif
};

extern Screen screen;

#endif
