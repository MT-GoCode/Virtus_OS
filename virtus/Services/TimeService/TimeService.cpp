#include "TimeService.h"
#include "EasySerial.h"

TimeService time_service;

RTC_DATA_ATTR bool time_set_since_cold_boot = false;
RTC_DATA_ATTR int32_t tz_offset;

int TimeService::handle_first_time_boot() {
    es.println("enteringn handle_first_time_boot");
    sync_via_bluetooth(100);

    return 0;
}

int TimeService::sync_via_bluetooth(int timeout = 10) {
    es.println("entering sync via bluetooth");
    ServerConfig config = {
        BT_TIMESERVICE,
        {{ BT_TIMESERVICE_CHAR_RX, "W", bt_on_write_time, nullptr}, { BT_TIMESERVICE_REQUEST, "N", nullptr, nullptr }},
        bt_on_connect,
        bt_on_disconnect
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
    // es.println("killing server");
    // bt_server.kill_server();

    if (time_set_since_cold_boot){
        es.println("sync successful. continuing");
        return 0;
    }
    else{
        es.println("sync unsuccessful.");
        return 1;
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