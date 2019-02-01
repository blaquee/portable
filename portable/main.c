//
//  main.c
//  portable
//
//  Created by Greg Lindor on 1/15/19.
//  Copyright © 2019 Greg Lindor. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <libproc.h>
#include <sys/proc_info.h>
#include <sys/kernel_types.h>
#include <mach/mach.h>
#include <mach/port.h>
#include "ports.h"
#define CHECK_MACH_RETURN(kr, msg) if(kr != KERN_SUCCESS) {do {printf("[-] %s: 0x%x\n", msg, kr); exit(kr); } while(0);}

typedef struct _proc_info_list
{
    struct _proc_info_list* next;
    char name[256];
    uint32_t pid;
    task_t task;
    boolean_t has_send_ports;
    size_t scount; // send port counts
}proc_info_list;

// global linked list
proc_info_list* g_procs_list_head;

void proc_list_initialize(proc_info_list* head)
{
    head = (proc_info_list*)malloc(sizeof(proc_info_list));
    if(!head)
    {
        printf("bad memory!\n");
        exit(0);
    }
    head->next = head;
    return;
}

void append_proc_list(proc_info_list* head, proc_info_list* entry)
{
    
}

char* ptype_to_string(mach_port_type_t type)
{
    char *str = (char*)malloc(256);
    strcpy(str, "RIGHTS- ");
    if (type & MACH_PORT_TYPE_SEND)      { strcat(str,"SEND ");      }
    if (type & MACH_PORT_TYPE_RECEIVE)   { strcat(str,"RECEIVE ");   }
    if (type & MACH_PORT_TYPE_SEND_ONCE) { strcat(str,"SEND_ONCE "); }
    if (type & MACH_PORT_TYPE_PORT_SET)  { strcat(str,"PORT_SET ");  }
    if (type & MACH_PORT_TYPE_DEAD_NAME) { strcat(str,"DEAD_NAME "); }
    if (type & MACH_PORT_TYPE_DNREQUEST) { strcat(str,"DNREQUEST "); }
    return str;
}

kern_return_t populate_pids(proc_info_list* head, pid_t pid)
{
    struct proc_bsdinfo bsd_info;
    proc_info_list * plist;
    task_t cur_task;
    int index;
    //mach_port_name_t port_name;
    mach_port_name_array_t names_array;
    mach_port_type_array_t port_array;
    mach_msg_type_number_t ncount, tcount;
    //mach_port_limits_t     port_limits;
    mach_port_status_t     port_status;
    mach_msg_type_number_t port_info_count;
    kern_return_t kr;
    
    plist = (proc_info_list*)malloc(sizeof(proc_info_list));
    if(!plist)
    {
        printf("bad memory!\n");
        return KERN_FAILURE;
    }
    
    proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &bsd_info, PROC_PIDTBSDINFO_SIZE);
    plist->pid = bsd_info.pbi_pid;
    plist->task = 0;
    strcpy(plist->name, bsd_info.pbi_name);
    
    // elevate for this or task_for_pid entitlement
    kr = task_for_pid(mach_task_self(), plist->pid, &cur_task);
    if(kr != KERN_SUCCESS)
    {
        printf("Failed to task_for_pid!\n");
        return kr;
    }
    
    // get the mach port names for this process
    kr = mach_port_names(cur_task, &names_array, &ncount, &port_array, &tcount);
    if(kr != KERN_SUCCESS)
    {
        printf("Failure in mach_port_names!\n");
        return kr;
    }
    
    // Lets get the type information for each port in the names array
    printf("Procname: %s:\n", plist->name);
    printf("%8s %10s %10s %10s\n",
           "port", "msg_count", "can_send", "port type");
    
    for(index = 0; index < ncount; index++)
    {
        // iterating the names array and grabbing attributes for
        // each name
        port_info_count = MACH_PORT_RECEIVE_STATUS_COUNT;
        kr = mach_port_get_attributes(cur_task, names_array[index],
                                      MACH_PORT_RECEIVE_STATUS,
                                      (mach_port_info_t)&port_status,
                                      &port_info_count);
        if(kr != KERN_SUCCESS)
        {
            printf("no attr - %d\n", index);
            continue;
        }
        // print the information about the ports
        char *port_string = ptype_to_string(port_array[index]);
        printf("%8d %10d %10d %s\n",
               names_array[index],
               port_status.mps_mscount,
               port_status.mps_srights,
               port_string);
        free(port_string);
    }
    
    // now we need to get the bootstrap service associated with these processes.
    // deallocate the kernels copy-in memory
    vm_deallocate(mach_task_self(), (vm_address_t)names_array, ncount * sizeof(mach_port_name_t));
    vm_deallocate(mach_task_self(), (vm_address_t)port_array, tcount * sizeof(mach_port_type_t));
    return kr;
}

int main(int argc, const char * argv[])
{
    
    pid_t *pids = NULL;
    unsigned long pid_size = 0;
    int pids_count = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
    
    // init
    proc_list_initialize(g_procs_list_head);
    initialize_bootstrap();
    
    // allocate space for pids
    printf("Pids returned: %d\n", pids_count);
    pid_size = pids_count * sizeof(pid_t);
    pids = (pid_t*)malloc(pid_size);
    if(pids == 0)
    {
        printf("malloc failed\n");
        return 0;
    }
    // zero it.
    bzero(pids, pid_size);
    // call it again to get the pids
    proc_listpids(PROC_ALL_PIDS, 0, pids, (int)pid_size);
    
    for(int i = 0; i < pids_count; i++)
    {
        if(pids[i] == 0) continue;
        populate_pids(g_procs_list_head, pids[i]);
    }
    
    return 0;
}
