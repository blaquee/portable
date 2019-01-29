//
//  ports.h
//  portable
//
//  Created by Greg Lindor on 1/28/19.
//  Copyright © 2019 Greg Lindor. All rights reserved.
//

#ifndef ports_h
#define ports_h

#include <stdio.h>
#include <libproc.h>
#include <sys/proc_info.h>
#include <sys/kernel_types.h>
#include <mach/mach.h>
#include <servers/bootstrap.h>


mach_port_t g_bootstrap = MACH_PORT_NULL;

#endif /* ports_h */
