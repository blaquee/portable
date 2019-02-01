//
//  ports.c
//  portable
//
//  Created by Greg Lindor on 1/28/19.
//  Copyright © 2019 Greg Lindor. All rights reserved.
//

#include "ports.h"

bootstrap_t g_bootstrap = MACH_PORT_NULL;

bool initialize_bootstrap()
{
    // get the bootstrap port and save it globally
    task_get_bootstrap_port( mach_task_self(), &g_bootstrap);
    if(g_bootstrap != MACH_PORT_NULL)
        return true;
    return false;
}

bool port_knock(task_t t);
