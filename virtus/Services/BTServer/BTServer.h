#ifndef BTSERVER_H
#define BTSERVER_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <map>
#include <vector>
#include <string.h>
#include <string>
#include <WString.h>
#include "Services/BaseService.h"
#include "EasySerial.h"
#include "freertos/FreeRTOS.h"      // Add FreeRTOS header
#include "freertos/semphr.h"        // Add semaphore header

struct CharacteristicConfig {
    const char* uuid;
    const char* flags;
    void (*onWrite)();
    void (*onRead)();
};

struct ServerConfig {
    const char* service_uuid;
    std::vector<CharacteristicConfig> characteristics;
    void (*onConnect)();
    void (*onDisconnect)();
};

class BTServer : public BaseService {
public:
    BTServer() : server(nullptr), service(nullptr), lock_handle(nullptr) {
        lock_handle = xSemaphoreCreateBinary();  // Create semaphore in constructor
        xSemaphoreGive(lock_handle);            // Initially available
    }
    
    ~BTServer() {
        if (lock_handle) vSemaphoreDelete(lock_handle);  // Cleanup
    }

    int start_server(const ServerConfig& config);
    void kill_server();
    void set(const std::string& uuid, const std::string& value);
    void set(const std::string& uuid, uint16_t value);
    void set(const std::string& uuid, uint32_t value);
    void set(const std::string& uuid, int value);
    void set(const std::string& uuid, float value);
    void set(const std::string& uuid, double value);
    void notify(const std::string& uuid, const std::string& value);
    void notify(const std::string& uuid, uint16_t value);
    void notify(const std::string& uuid, uint32_t value);
    void notify(const std::string& uuid, int value);
    void notify(const std::string& uuid, float value);
    void notify(const std::string& uuid, double value);
    template<typename T> T peek(const String& uuid);
    bool can_notify(const std::string& uuid);

private:
    BLEServer* server;
    BLEService* service;
    std::map<std::string, BLECharacteristic*> char_map;
    std::map<std::string, bool> notify_flags;
    SemaphoreHandle_t lock_handle;  // Changed from bool to semaphore handle

    void clear_state();
};

extern BTServer bt_server;

template<typename T>
T BTServer::peek(const String& uuid) {
    T result{};
    if (uuid.isEmpty() || !uuid.c_str()) {
        es.println("Error: Invalid UUID passed to peek");
        return result;
    }
    std::string uuid_str = uuid.c_str();
    if (char_map.count(uuid_str)) {
        BLECharacteristic* ch = char_map[uuid_str];
        size_t len = ch->getLength();
        const uint8_t* raw = ch->getData();
        if (len > sizeof(T)) {
            es.println("Error: Data too long for " + uuid + ", got " + String(len) + ", max " + String(sizeof(T)));
            return result;
        }
        if (len > 0) {
            memcpy(&result, raw, len);
        } else {
            es.println("Warning: No data for " + uuid);
        }
    } else {
        es.println("Error: UUID not found: " + uuid);
    }
    return result;
}

template<> String BTServer::peek<String>(const String& uuid);

#endif // BTSERVER_H