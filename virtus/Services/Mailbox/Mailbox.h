#ifndef MAILBOX_H
#define MAILBOX_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "EasySerial.h"

template<typename T>
class Mailbox {
private:
    QueueHandle_t queue;
    static constexpr int default_depth = 10;

public:
    Mailbox() {
        queue = xQueueCreate(default_depth, sizeof(T));
        T dummy;
        while (xQueueReceive(queue, &dummy, 0) == pdTRUE) {} // Drain queue
    }

    ~Mailbox() {
        if (queue) vQueueDelete(queue);
    }

    bool write(const T& item) {
        return xQueueSend(queue, &item, 0) == pdTRUE;
    }

    bool read(T& out, int wait_time_ms = 0) {

        if (!queue) {
            es.println("read on uniitialized");
            return false;
        }
        TickType_t wait_ticks = (wait_time_ms > 0) ? pdMS_TO_TICKS(wait_time_ms) : 0;
        return xQueueReceive(queue, &out, wait_ticks) == pdTRUE;
    }

    Mailbox(const Mailbox&) = delete;
    Mailbox& operator=(const Mailbox&) = delete;
};

#endif