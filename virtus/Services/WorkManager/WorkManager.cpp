#include "WorkManager.h"

#include "Services/ScreenManager/ScreenManager.h"

int WorkManager::add_app_job(int id, time_t time) {
    if (!time) {
        screen_manager.mount_app(id);
        return 0;
    }
    return 0;
}