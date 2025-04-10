#ifndef APPREGISTRY_H
#define APPREGISTRY_H

#include "Apps/BaseApp.h"
#include "Apps/System/BIOS/BIOS.h"

inline BIOS first_time_boot_menu;

inline BaseApp* app_registry[] = {
    &first_time_boot_menu,
};

#endif