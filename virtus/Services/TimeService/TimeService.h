#ifndef TIMESERVICE_H
#define TIMESERVICE_H

#include <SensorPCF8563.hpp>
#include <time.h>

#include "Services/BaseService.h"
#include "Services/BTServer/BTServer.h"
#include "Services/Mailbox/Mailbox.h"

#define BT_TIMESERVICE "13371337-0000-4000-8000-133713371337"
#define BT_TIMESERVICE_CHAR_RX "13371337-0001-4000-8000-133713371337"

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

    void sync_via_bluetooth(int timeout, Mailbox<int>* mail_time_service);
    bool is_time_set();

    void bt_on_connect();
    void bt_on_disconnect();
    void bt_on_write_time();

    struct tm get_time(bool adjust_for_timezone = true);

protected:

};

extern TimeService time_service;

#endif