#include "Screen.h"

Screen::Screen() {} 

int Screen::handle_basic_boot() {

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
    ledcAttach(BOARD_TFT_BL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
#else
    ledcSetup(LEDC_BACKLIGHT_CHANNEL, LEDC_BACKLIGHT_FREQ, LEDC_BACKLIGHT_BIT_WIDTH);
    ledcAttachPin(BOARD_TFT_BL, LEDC_BACKLIGHT_CHANNEL);
#endif

    TFT_eSPI::init();
    setRotation(2);
    setTextDatum(MC_DATUM);
    setTextFont(2);
    fillScreen(TFT_BLACK);

    fillScreen(TFT_BLACK);
    drawString("Hello T-Watch", 120, 120);
    setBrightness(50);
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