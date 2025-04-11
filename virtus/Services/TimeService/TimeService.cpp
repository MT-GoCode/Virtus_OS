#include "TimeService.h"
#include "EasySerial.h"

TimeService time_service;

RTC_DATA_ATTR bool time_set_since_cold_boot = false;
RTC_DATA_ATTR int32_t tz_offset;

void TimeService::sync_via_bluetooth(int timeout, Mailbox<int>* mail_time_service) {
    es.println("entering sync via bluetooth");
    ServerConfig config = {
        BT_TIMESERVICE,
        {{ BT_TIMESERVICE_CHAR_RX, "W", [&]() { time_service.bt_on_write_time(); }, nullptr}},
        [&]() { time_service.bt_on_connect(); },
        [&]() { time_service.bt_on_disconnect(); }
    };
    es.println("config made");
    bt_server.start_server(config);

    unsigned long start = millis();
    const unsigned long timeout_ms = timeout * 1000;

    while (true) {
        if (millis() - start > timeout_ms || time_set_since_cold_boot) {
            break;
        }
        delay(10);
    }
    es.println("killing server");
    bt_server.kill_server();

    if (time_set_since_cold_boot){
        es.println("sync successful. continuing");
        mail_time_service->write(0);
    }
    else{
        es.println("sync unsuccessful.");
        mail_time_service->write(1);
    }
}

void TimeService::bt_on_write_time() {
    es.println("written!");

    TimePayload payload = bt_server.peek<TimePayload>(BT_TIMESERVICE_CHAR_RX);

    String datetime = String(payload.year) + "-" +
                  (payload.month < 10 ? "0" : "") + String(payload.month) + "-" +
                  (payload.day < 10 ? "0" : "") + String(payload.day) + " " +
                  (payload.hour < 10 ? "0" : "") + String(payload.hour) + ":" +
                  (payload.minute < 10 ? "0" : "") + String(payload.minute) + ":" +
                  (payload.second < 10 ? "0" : "") + String(payload.second);
    es.println(datetime);

    time_service.setDateTime(payload.year, payload.month, payload.day, payload.hour, payload.minute, payload.second);
    
    tz_offset = payload.tz_offset;

    es.println(payload.tz_offset);
        
    time_set_since_cold_boot = true;
}

void TimeService::bt_on_connect() { 
    es.println("COnnected");

}

void TimeService::bt_on_disconnect() { 
    es.println("Disconnected!");
}

bool TimeService::is_time_set() {   
    return time_set_since_cold_boot;
}

struct tm TimeService::get_time(bool adjust_for_timezone) {
    struct tm result = {0};  // zero initialize

    RTC_DateTime now = getDateTime();
    if (!now.available) {
        return result;  // return zeroed struct if invalid
    }

    // Fill base time (RTC is assumed to be in UTC or local base)
    result.tm_year = now.year - 1900;
    result.tm_mon  = now.month - 1;
    result.tm_mday = now.day;
    result.tm_hour = now.hour;
    result.tm_min  = now.minute;
    result.tm_sec  = now.second;
    result.tm_isdst = 0;

    if (adjust_for_timezone) {
        time_t raw = mktime(&result);
        raw += tz_offset;
        result = *localtime(&raw);  // localtime returns pointer to static
    }

    return result;
}