#ifndef WORKMANAGER_H
#define WORKMANAGER_H

#include <functional>
#include <unordered_set>
#include "Time.h" // for time_t
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

typedef std::function<void()> JobFunc;

class WorkManager {
public:
    int add_app_job(int id, time_t time = 0);
    TaskHandle_t add_cpu_job(JobFunc func, time_t time = 0);

    int step();
    bool cpu_jobs_inactive();

private:
    std::unordered_set<TaskHandle_t> job_set;
    SemaphoreHandle_t job_set_mutex = xSemaphoreCreateMutex();

    void add_job(TaskHandle_t handle);
    void remove_job(TaskHandle_t handle);
    static void job_wrapper(void* param);

};

extern WorkManager work_manager;

#endif // WORKMANAGER_H
