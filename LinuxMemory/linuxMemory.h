#define _GNU_SOURCE

#ifndef LINUXMEMORY_H
#define LINUXMEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <fcntl.h>


typedef struct linuxProc {
    char* procMempath;
    pid_t procId;
} linuxProc;

#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE 0
#define CASE_INSENSITIVE 1
#define EXACT_MATCH 1
#define INEXACT_MATCH 0

int attach(linuxProc target);
int detach(linuxProc target);
int procRead(linuxProc Process, int32_t nsize, void* address, void* buffer);
pid_t GetPIDbyName(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch);
int isNumeric(const char* ccharptr_CharacterList);
linuxProc linuxProcFromID(pid_t pid);

#endif