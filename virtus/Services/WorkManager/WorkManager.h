#ifndef WORKMANAGER_H
#define WORKMANAGER_H

#include "Time.h" // gets access to time_t

typedef void (*JobFunc)();

class WorkManager {
public:
    int add_app_job(int id, time_t time = 0);
    int add_cpu_job(JobFunc func, time_t time = 0); // if persisting through deep sleep, function must be static & global lest it won't work

    int step();
    bool cpu_jobs_inactive();
};

extern WorkManager work_manager;

#endif 