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

typedef struct _proc_info_list
{
    struct _proc_info_list* next;
    char name[256];
    uint32_t pid;
}proc_info_list;

void print_pids(pid_t pid)
{
    struct proc_bsdinfo bsd_info;
    //struct proc_bsdshortinfo bsd_sinfo;
    
    proc_pidinfo(pid, PROC_PIDTBSDINFO, 0, &bsd_info, PROC_PIDTBSDINFO_SIZE);
    //proc_pidinfo(pid, PROC_PIDT_SHORTBSDINFO, 0, &bsd_sinfo, PROC_PIDT_SHORTBSDINFO_SIZE);
    printf("%s\t %u\n", bsd_info.pbi_name, bsd_info.pbi_pid);
}

int main(int argc, const char * argv[])
{
    
    pid_t *pids = NULL;
    unsigned long pid_size = 0;
    
    int pids_count = proc_listpids(PROC_ALL_PIDS, 0, NULL, 0);
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
    proc_listpids(PROC_ALL_PIDS, 0, pids, pid_size);
    printf("%16s\t %4s\n", "process", "pid");
    
    for(int i = 0; i < pids_count; i++)
    {
        if(pids[i] == 0) continue;
        //printf("%i: %i\n", i, pids[i]);
        print_pids(pids[i]);
    }
    return 0;
}
