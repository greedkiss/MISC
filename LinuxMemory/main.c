#include <stdio.h>

#include "linuxMemory.h"

int main()
{
    linuxProc proc;
    char name[20];
    scanf("%s", name);

    proc = linuxProcFromID((pid_t)GetPIDbyName(name, 1, 0));
    printf("process id: %i\n", proc.procId);

    void* address = (void*)0x55841acd3001;
    attach(proc);
    procRead(proc, 0, address, 0);
    detach(proc);

    return 0;
}