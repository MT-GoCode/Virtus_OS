#ifndef APPREGISTRY_H
#define APPREGISTRY_H

#include "Apps/BaseApp.h"
#include "Apps/System/BIOS/BIOS.h"
#include "Apps/System/DefaultWatchFace/DefaultWatchFace.h"

inline BaseApp* app_registry[] = {
    &bios, &default_watch_face
};

#endif