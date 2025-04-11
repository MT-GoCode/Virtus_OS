#include "Battery.h"

Battery battery;

int Battery::handle_basic_boot() {

    bool res =  XPowersAXP2101::init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL);
    if (!res) {
        return 1;
    }

    // Set the minimum common working voltage of the PMU VBUS input,
    // below this value will turn off the PMU
    setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);

    // Set the maximum current of the PMU VBUS input,
    // higher than this value will turn off the PMU
    setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_900MA);

    // Set VSY off voltage as 2600mV , Adjustment range 2600mV ~ 3300mV
    setSysPowerDownVoltage(2600);


    // ! ESP32S3 VDD, Don't change
    // setDC1Voltage(3300);

    //! RTC VBAT , Don't change
    setALDO1Voltage(3300);

    //! TFT BACKLIGHT VDD , Don't change
    setALDO2Voltage(3300);

    //!Screen touch VDD , Don't change
    setALDO3Voltage(3300);

    //! Radio VDD , Don't change
    setALDO4Voltage(3300);

    //!DRV2605 enable
    setBLDO2Voltage(3300);

    //! GPS Power
    setDC3Voltage(3300);
    enableDC3();

    //! No use
    disableDC2();
    // disableDC3();
    disableDC4();
    disableDC5();
    disableBLDO1();
    disableCPUSLDO();
    disableDLDO1();
    disableDLDO2();



    enableALDO1();  //! RTC VBAT
    enableALDO2();  //! TFT BACKLIGHT   VDD
    enableALDO3();  //! Screen touch VDD
    enableALDO4();  //! Radio VDD
    enableBLDO2();  //! drv2605 enable


    // if (stream) {
    //     log_println("DCDC=======================================================================");
    //     stream->printf("DC1  : %s   Voltage:%u mV \n", isEnableDC1()  ? "+" : "-", getDC1Voltage());
    //     stream->printf("DC2  : %s   Voltage:%u mV \n", isEnableDC2()  ? "+" : "-", getDC2Voltage());
    //     stream->printf("DC3  : %s   Voltage:%u mV \n", isEnableDC3()  ? "+" : "-", getDC3Voltage());
    //     stream->printf("DC4  : %s   Voltage:%u mV \n", isEnableDC4()  ? "+" : "-", getDC4Voltage());
    //     stream->printf("DC5  : %s   Voltage:%u mV \n", isEnableDC5()  ? "+" : "-", getDC5Voltage());
    //     log_println("ALDO=======================================================================");
    //     stream->printf("ALDO1: %s   Voltage:%u mV\n", isEnableALDO1()  ? "+" : "-", getALDO1Voltage());
    //     stream->printf("ALDO2: %s   Voltage:%u mV\n", isEnableALDO2()  ? "+" : "-", getALDO2Voltage());
    //     stream->printf("ALDO3: %s   Voltage:%u mV\n", isEnableALDO3()  ? "+" : "-", getALDO3Voltage());
    //     stream->printf("ALDO4: %s   Voltage:%u mV\n", isEnableALDO4()  ? "+" : "-", getALDO4Voltage());
    //     log_println("BLDO=======================================================================");
    //     stream->printf("BLDO1: %s   Voltage:%u mV\n", isEnableBLDO1()  ? "+" : "-", getBLDO1Voltage());
    //     stream->printf("BLDO2: %s   Voltage:%u mV\n", isEnableBLDO2()  ? "+" : "-", getBLDO2Voltage());
    //     log_println("CPUSLDO====================================================================");
    //     stream->printf("CPUSLDO: %s Voltage:%u mV\n", isEnableCPUSLDO() ? "+" : "-", getCPUSLDOVoltage());
    //     log_println("DLDO=======================================================================");
    //     stream->printf("DLDO1: %s   Voltage:%u mV\n", isEnableDLDO1()  ? "+" : "-", getDLDO1Voltage());
    //     stream->printf("DLDO2: %s   Voltage:%u mV\n", isEnableDLDO2()  ? "+" : "-", getDLDO2Voltage());
    //     log_println("===========================================================================");
    // }


    // Set the time of pressing the button to turn off
    setPowerKeyPressOffTime(XPOWERS_POWEROFF_4S);

    // Set the button power-on press time
    setPowerKeyPressOnTime(XPOWERS_POWERON_512MS);


    // It is necessary to disable the detection function of the TS pin on the board
    // without the battery temperature detection function, otherwise it will cause abnormal charging
    disableTSPinMeasure();


    // Enable internal ADC detection
    enableBattDetection();
    enableVbusVoltageMeasure();
    enableBattVoltageMeasure();
    enableSystemVoltageMeasure();


    //t-watch no chg led
    setChargingLedMode(XPOWERS_CHG_LED_OFF);


    disableIRQ(XPOWERS_AXP2101_ALL_IRQ);

    /*
    Battery insert/remove.
    USB plug/unplug.
    Short/long power button press.
    Charging start/done.
    These pull BOARD_PMU_INT low when triggered.
    */

    // Enable the required interrupt function
    enableIRQ(
        XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |   //POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
    );

    // Clear all interrupt flags
    XPowersAXP2101::clearIrqStatus();

    // Set the precharge charging current
    setPrechargeCurr(XPOWERS_AXP2101_PRECHARGE_50MA);
    // Set constant current charge current limit
    setChargerConstantCurr(XPOWERS_AXP2101_CHG_CUR_300MA);
    // Set stop charging termination current
    setChargerTerminationCurr(XPOWERS_AXP2101_CHG_ITERM_25MA);

    // Set charge cut-off voltage
    setChargeTargetVoltage(XPOWERS_AXP2101_CHG_VOL_4V35);

    // Set RTC Battery voltage to 3.3V
    setButtonBatteryChargeVoltage(3300);

    enableButtonBatteryCharge();

    disableIRQ(XPOWERS_AXP2101_ALL_IRQ);

    enableIRQ(
        // XPOWERS_AXP2101_BAT_INSERT_IRQ    | XPOWERS_AXP2101_BAT_REMOVE_IRQ      |   //BATTERY
        XPOWERS_AXP2101_VBUS_INSERT_IRQ   | XPOWERS_AXP2101_VBUS_REMOVE_IRQ     |   //VBUS
        XPOWERS_AXP2101_PKEY_SHORT_IRQ    | XPOWERS_AXP2101_PKEY_LONG_IRQ       |  //POWER KEY
        XPOWERS_AXP2101_BAT_CHG_DONE_IRQ  | XPOWERS_AXP2101_BAT_CHG_START_IRQ       //CHARGE
        // XPOWERS_AXP2101_PKEY_NEGATIVE_IRQ | XPOWERS_AXP2101_PKEY_POSITIVE_IRQ   |   //POWER KEY
    );

    return 0;
}