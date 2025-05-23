#pragma once

#define BOARD_TFT_WIDTH             (240)
#define BOARD_TFT_HEIGHT             (240)

// ST7789
#define BOARD_TFT_MISO              (-1)
#define BOARD_TFT_MOSI              (13)
#define BOARD_TFT_SCLK              (18)
#define BOARD_TFT_CS                (12)
#define BOARD_TFT_DC                (38)
#define BOARD_TFT_RST               (-1)
#define BOARD_TFT_BL                (45)

// Touch
#define BOARD_TOUCH_SDA             (39)
#define BOARD_TOUCH_SCL             (40)
#define BOARD_TOUCH_INT             (16)

//BMA423,PCF8563,AXP2101,DRV2605L
#define BOARD_I2C_SDA               (10)
#define BOARD_I2C_SCL               (11)

// PCF8563 Interrupt
#define BOARD_RTC_INT_PIN           (17)
// AXP2101 Interrupt
#define BOARD_PMU_INT               (21)
// BMA423 Interrupt
#define BOARD_BMA423_INT1           (14)

// MAX98357A
#define BOARD_DAC_IIS_BCK           (48)
#define BOARD_DAC_IIS_WS            (15)
#define BOARD_DAC_IIS_DOUT          (46)
