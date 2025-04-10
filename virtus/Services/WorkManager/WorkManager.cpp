#include "WorkManager.h"
#include "Services/ScreenManager/ScreenManager.h"

WorkManager work_manager;

int WorkManager::add_app_job(int id, time_t time) {
    if (!time) {
        screen_manager.mount_app(id);
        return 0;
    }
    // TODO: Delayed app job scheduling
    return 0;
}

void WorkManager::add_job(TaskHandle_t handle) {
    xSemaphoreTake(job_set_mutex, portMAX_DELAY);
    job_set.insert(handle);
    xSemaphoreGive(job_set_mutex);
}

void WorkManager::remove_job(TaskHandle_t handle) {
    xSemaphoreTake(job_set_mutex, portMAX_DELAY);
    job_set.erase(handle);
    xSemaphoreGive(job_set_mutex);
}

void WorkManager::job_wrapper(void* param) {
    auto* tuple = static_cast<std::tuple<WorkManager*, JobFunc>*>(param);
    WorkManager* self = std::get<0>(*tuple);
    JobFunc func = std::get<1>(*tuple);

    TaskHandle_t handle = xTaskGetCurrentTaskHandle();
    self->add_job(handle);

    func();

    self->remove_job(handle);
    delete tuple;
    vTaskDelete(nullptr);
}


int WorkManager::add_cpu_job(JobFunc func, time_t time) {
    if (!func) return -1;

    if (time == 0) {
        auto* data = new std::tuple<WorkManager*, JobFunc>(this, func);
        BaseType_t result = xTaskCreate(
            job_wrapper,
            "cpu_job",
            4096, // Stack size
            data,
            1,    // Priority
            nullptr
        );
        return (result == pdPASS) ? 0 : -2;
    }

    // TODO: support delayed jobs
    return -1;
}

bool WorkManager::cpu_jobs_inactive() {
    xSemaphoreTake(job_set_mutex, portMAX_DELAY);
    bool inactive = job_set.empty();
    xSemaphoreGive(job_set_mutex);
    return inactive;
}
