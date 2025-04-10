#ifndef BTSERVERMANAGER_H
#define BTSERVERMANAGER_H

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

// Config for individual characteristics
struct CharacteristicConfig {
    const char* uuid;
    const char* flags; // RWN
    void (*onWrite)();
    void (*onRead)();
};

// Config for the server
struct ServerConfig {
    const char* service_uuid;
    std::vector<CharacteristicConfig> characteristics;
    void (*onConnect)();
    void (*onDisconnect)();
};

class BTServer {
public:
    // Set (no notify)
    void set(const std::string& uuid, const std::string& value);
    void set(const std::string& uuid, uint16_t value);
    void set(const std::string& uuid, uint32_t value);
    void set(const std::string& uuid, int value);
    void set(const std::string& uuid, float value);
    void set(const std::string& uuid, double value);

    // Notify (only if allowed)
    void notify(const std::string& uuid, const std::string& value);
    void notify(const std::string& uuid, uint16_t value);
    void notify(const std::string& uuid, uint32_t value);
    void notify(const std::string& uuid, int value);
    void notify(const std::string& uuid, float value);
    void notify(const std::string& uuid, double value);

    // Get typed data
    template<typename T>
    T get(const String& uuid);

    bool can_notify(const std::string& uuid);

private:
    BLEServer* server = nullptr;
    BLEService* service = nullptr;
    std::map<std::string, BLECharacteristic*> char_map;
    std::map<std::string, bool> notify_flags;

    friend class BTServerManager;
};

class BTServerManager : BaseService {
    friend class System;
public:
    BTServer* create_server(const ServerConfig& config);
};

extern BTServerManager bt_server_manager;
// Template definitions outside the class
template<typename T>
T BTServer::get(const String& uuid) {
    T result{};  // Zero-initialize for safety
    std::string uuid_str = uuid.c_str();
    if (char_map.count(uuid_str)) {
        BLECharacteristic* ch = char_map[uuid_str];
        size_t len = ch->getLength();
        const uint8_t* raw = ch->getData();

        if (len > sizeof(T)) {
            es.println("Error: Data too long for " + uuid + ", got " + String(len) + ", max " + String(sizeof(T)));
            return result;  // Or throw an exception if you prefer
        }

        // Copy whatever we have, up to sizeof(T)
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


#endif // BTSERVERMANAGER_H
