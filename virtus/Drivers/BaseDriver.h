#ifndef BASEDRIVER_H
#define BASEDRIVER_H

#include "utilities.h"

class  BaseDriver {
public:
protected:
    virtual int handle_basic_boot() { return 0; }
    virtual int handle_first_time_boot() { return 0; }
    virtual int handle_os_init() { return 0; }
    virtual int handle_active_loop() { return 0; }

};

#endif