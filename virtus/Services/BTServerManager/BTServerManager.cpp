#include "BTServerManager.h"

#include "esp_gatt_common_api.h"

#include "EasySerial.h"

BTServerManager bt_server_manager;

BTServer* BTServerManager::create_server(const ServerConfig& config) {
    BTServer instance;
    BLEDevice::init("BTServer");
    esp_ble_gatt_set_local_mtu(100);

    if (!config.service_uuid || config.characteristics.empty()) return nullptr;

    instance.server = BLEDevice::createServer();
    if (!instance.server) return nullptr;

    class InternalCallbacks : public BLEServerCallbacks {
    public:
        InternalCallbacks(void (*onC)(), void (*onD)())
            : _onConnect(onC), _onDisconnect(onD) {}

        void onConnect(BLEServer* pServer) override {
            if (_onConnect) _onConnect();
        }

        void onDisconnect(BLEServer* pServer) override {
            if (_onDisconnect) _onDisconnect();
        }

    private:
        void (*_onConnect)();
        void (*_onDisconnect)();
    };

    instance.server->setCallbacks(new InternalCallbacks(config.onConnect, config.onDisconnect));
    instance.service = instance.server->createService(config.service_uuid);
    if (!instance.service) return nullptr;

    for (const auto& c : config.characteristics) {
        if (!c.uuid || !c.flags || strlen(c.uuid) < 4)
            return nullptr;
        
        es.println("Processing UUID: " + String(c.uuid) + " with flags: " + String(c.flags));
        es.println(strchr(c.flags, 'W'));

        uint32_t props = 0;
        if (strchr(c.flags, 'R')) props |= BLECharacteristic::PROPERTY_READ;
        if (strchr(c.flags, 'W')) props |= BLECharacteristic::PROPERTY_WRITE;
        if (strchr(c.flags, 'N')) props |= BLECharacteristic::PROPERTY_NOTIFY;

        BLECharacteristic* ch = instance.service->createCharacteristic(c.uuid, props);
        if (!ch) return nullptr;
        es.println("Created characteristic: " + String(c.uuid) + " with props: " + String(props, HEX));

        class CharCallbacks : public BLECharacteristicCallbacks {
        public:
            CharCallbacks(void (*onW)(), void (*onR)()) : _onWrite(onW), _onRead(onR) {}

            void onWrite(BLECharacteristic* c) override {
                if (_onWrite) _onWrite();
            }

            void onRead(BLECharacteristic* c) override {
                if (_onRead) _onRead();
            }

        private:
            void (*_onWrite)();
            void (*_onRead)();
        };

        ch->setCallbacks(new CharCallbacks(c.onWrite, c.onRead));
        instance.char_map[c.uuid] = ch;
        instance.notify_flags[c.uuid] = strchr(c.flags, 'N') != nullptr;
    }

    instance.service->start();
    BLEAdvertising* adv = BLEDevice::getAdvertising();
    adv->addServiceUUID(config.service_uuid);
    adv->start();

    return new BTServer(std::move(instance)); // optional: use new here to persist
}

// Setters (no notify)
void BTServer::set(const std::string& uuid, const std::string& value) {
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, uint16_t value) {
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, uint32_t value) {
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, int value) {
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, float value) {
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

void BTServer::set(const std::string& uuid, double value) {
    if (char_map.count(uuid)) char_map[uuid]->setValue(value);
}

// Notify (set + notify if allowed)
void BTServer::notify(const std::string& uuid, const std::string& value) {
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, uint16_t value) {
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, uint32_t value) {
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, int value) {
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, float value) {
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

void BTServer::notify(const std::string& uuid, double value) {
    if (char_map.count(uuid) && notify_flags[uuid]) {
        char_map[uuid]->setValue(value);
        char_map[uuid]->notify();
    }
}

bool BTServer::can_notify(const std::string& uuid) {
    return notify_flags.count(uuid) && notify_flags[uuid];
}

// Specialization for String
template<>
String BTServer::get<String>(const String& uuid) {
    String result;
    std::string uuid_str = uuid.c_str();
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