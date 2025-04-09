#ifndef APPREGISTRY_H
#define APPREGISTRY_H

#include "Apps/BaseApp.h"
#include "Apps/System/FirstTimeBootMenu/FirstTimeBootMenu.h"

inline FirstTimeBootMenu first_time_boot_menu;

inline BaseApp* app_registry[] = {
    &first_time_boot_menu,
};

#endif