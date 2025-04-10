#include "Screen.h"
#include "Drivers/Screen/LV/LV_Helper.h"

Screen screen;

int Screen::handle_basic_boot() {

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
    ledcAttach(BOARD_TFT_BL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
#else
    ledcSetup(LEDC_BACKLIGHT_CHANNEL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
    ledcAttachPin(BOARD_TFT_BL, LEDC_BACKLIGHT_CHANNEL);
#endif

    TFT_eSPI::init();
    setBrightness(50);
    setRotation(2);
    fillScreen(TFT_BLACK);

    TouchDrvFT6X36::begin(Wire1, FT6X36_SLAVE_ADDRESS, BOARD_TOUCH_SDA, BOARD_TOUCH_SCL);
    interruptTrigger();

    beginLvglHelper(false);
    return 0;
}

void Screen::setBrightness(uint8_t level)
{
    if (!level) {
        battery.disableALDO2();
        writecommand(0x10);  //display sleep
        setPowerMode(PMODE_MONITOR);
    }
    if (!brightness && level != 0) {
        battery.enableALDO2();
        writecommand(0x11);  //display wakeup
    }
    brightness = level;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
    ledcWrite(BOARD_TFT_BL, brightness);
#else
    ledcWrite(LEDC_BACKLIGHT_CHANNEL, brightness);
#endif
}