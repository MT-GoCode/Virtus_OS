#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <SensorPCF8563.hpp>

#include "Services/BaseService.h"
#include "Services/BTServerManager/BTServerManager.h"

#define BT_TIMESERVICE "13371337-0000-4000-8000-133713371337"
#define BT_TIMESERVICE_CHAR_RX "13371337-0001-4000-8000-133713371337"
#define BT_TIMESERVICE_REQUEST "13371337-0002-4000-8000-133713371337"

#pragma pack(push, 1)
struct TimePayload {
    uint16_t year;
    int32_t month;
    int32_t day;
    int32_t hour;
    int32_t minute;
    int32_t second;
    int32_t tz_offset;
};

class TimeService : BaseService, public SensorPCF8563 {
    friend class System;
public:

    int sync_via_bluetooth(int timeout);
    bool is_time_set();

    static void bt_on_connect();
    static void bt_on_disconnect();
    static void bt_on_write_time();

protected:
    virtual int handle_first_time_boot();

private:    
    static BTServer* bts;

};

extern TimeService time_service;

#endif