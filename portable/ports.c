//
//  ports.c
//  portable
//
//  Created by Greg Lindor on 1/28/19.
//  Copyright © 2019 Greg Lindor. All rights reserved.
//

#include "ports.h"


bool initialize_bootstrap()
{
    // get the bootstrap port and save it globally
    task_get_bootstrap_port( mach_self_task(), &g_bootstrap);
}

bool can_lookup_port(char* name)
{
    // determine if we can perform a lookup through the bootstrap service (launchd)
    
}
