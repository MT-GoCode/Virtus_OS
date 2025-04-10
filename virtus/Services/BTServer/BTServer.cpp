#include "BTServer.h"
#include "esp_gatt_common_api.h"
#include "EasySerial.h"

BTServer bt_server;

int BTServer::start_server(const ServerConfig& config) {
    if (xSemaphoreTake(lock_handle, 10) != pdTRUE) return -1;
    
    if (!config.service_uuid || config.characteristics.empty()) {
        xSemaphoreGive(lock_handle);
        return -2;
    }

    clear_state();
    BLEDevice::init("BTServer");
    esp_ble_gatt_set_local_mtu(100);

    server = BLEDevice::createServer();
    if (!server) {
        xSemaphoreGive(lock_handle);
        return -2;
    }

    class InternalCallbacks : public BLEServerCallbacks {
    public:
        InternalCallbacks(BLEServer* serverPtr,
                            std::function<void()> onC,
                            std::function<void()> onD)
            : _server(serverPtr), _onConnect(onC), _onDisconnect(onD) {}
    
        void onConnect(BLEServer* pServer) override {
            es.println("Connected! MTU: " + String(_server->getPeerMTU(_server->getConnId())));
            if (_onConnect) _onConnect();
        }
    
        void onDisconnect(BLEServer* pServer) override {
            if (_onDisconnect) _onDisconnect();
        }
    
    private:
        BLEServer* _server;
        std::function<void()> _onConnect;
        std::function<void()> _onDisconnect;
    };
        

    server->setCallbacks(new InternalCallbacks(server, config.onConnect, config.onDisconnect));
    service = server->createService(config.service_uuid);
    if (!service) {
        clear_state();
        xSemaphoreGive(lock_handle);
        return -2;
    }

    for (const auto& c : config.characteristics) {
        if (!c.uuid || !c.flags || strlen(c.uuid) < 4) {
            clear_state();
            xSemaphoreGive(lock_handle);
            return -2;
        }

        es.println("---- Characteristic Setup ----");
        es.println("UUID  : " + String(c.uuid));
        es.println("Flags : " + String(c.flags));

        uint32_t props = 0;
        // Check and apply flags
        if (strchr(c.flags, 'R')) {
            props |= BLECharacteristic::PROPERTY_READ;
            es.println(" → Adding PROPERTY_READ");
        }
        if (strchr(c.flags, 'W')) {
            props |= BLECharacteristic::PROPERTY_WRITE;
            es.println(" → Adding PROPERTY_WRITE");
        }
        if (strchr(c.flags, 'N')) {
            props |= BLECharacteristic::PROPERTY_NOTIFY;
            es.println(" → Adding PROPERTY_NOTIFY");
        }

        char buf[32];
        sprintf(buf, "Final props bitmask: 0x%X", props);
        es.println(String(buf));

        es.println("-----------------------------");

        BLECharacteristic* ch = service->createCharacteristic(c.uuid, props);
        if (!ch) {
            clear_state();
            xSemaphoreGive(lock_handle);
            return -2;
        }

        class CharCallbacks : public BLECharacteristicCallbacks {
        public:
            CharCallbacks(std::function<void()> onW, std::function<void()> onR)
                : _onWrite(onW), _onRead(onR) {}
        
            void onWrite(BLECharacteristic* c) override {
                if (_onWrite) _onWrite();
            }
        
            void onRead(BLECharacteristic* c) override {
                if (_onRead) _onRead();
            }
        
        private:
            std::function<void()> _onWrite;
            std::function<void()> _onRead;
        };
            

        ch->setCallbacks(new CharCallbacks(c.onWrite, c.onRead));
        char_map[c.uuid] = ch;
        notify_flags[c.uuid] = strchr(c.flags, 'N') != nullptr;
    }

    service->start();
    BLEAdvertising* adv = BLEDevice::getAdvertising();
    adv->addServiceUUID(config.service_uuid);
    adv->start();

    return 0;  // Success, lock remains taken
}

void BTServer::kill_server() {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }

    BLEAdvertising* adv = BLEDevice::getAdvertising();
    if (adv) adv->stop();  // Stop advertising to prevent new connections
    
    clear_state();
    xSemaphoreGive(lock_handle);
}

void BTServer::clear_state() {
    if (service) {
        service->stop();
        server->removeService(service);
        service = nullptr;
    }
    if (server) {
        delete server;
        server = nullptr;
    }
    char_map.clear();
    notify_flags.clear();
}

// Setters (no notify)
void BTServer::set(const std::string& uuid, const std::string& value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, uint16_t value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, uint32_t value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, int value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, float value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, double value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

// Notify (set + notify if allowed)
void BTServer::notify(const std::string& uuid, const std::string& value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, uint16_t value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, uint32_t value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, int value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, float value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, double value) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return;
    }
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

bool BTServer::can_notify(const std::string& uuid) {
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return false;
    }
    return notify_flags.count(uuid) && notify_flags[uuid];
}

template<>
String BTServer::peek<String>(const String& uuid) {
    String result;
    std::string uuid_str = uuid.c_str();
    if (xSemaphoreTake(lock_handle, 0) == pdTRUE) {
        xSemaphoreGive(lock_handle);
        return result;  // Empty string if server not running
    }
    if (char_map.count(uuid_str)) {
        BLECharacteristic* ch = char_map[uuid_str];
        size_t len = ch->getLength();
        const uint8_t* raw = ch->getData();
        if (len > 0) {
            result = String(reinterpret_cast<const char*>(raw));
        }
    }
    return result;
}