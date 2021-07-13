#include "linuxMemory.h"

int isNumeric(const char* charterList)
{
    for( ;*charterList; charterList++){
        if(*charterList < '0' || *charterList > '9')
            return 0;
    }
    return 1;
}

int strcmp_ws(const char* s1, const char* s2, int intCaseSensitive)
{
    if(intCaseSensitive)
        return !strcmp(s1, s2);
    else
        return !strcasecmp(s1, s2);
}

int strstr_ws(const char* haystack, const char* needle, int intCaseSensitive)
{
    if (intCaseSensitive)
        return (int) strstr(haystack, needle);
    else
        return (int) strcasestr(haystack, needle);
}

pid_t GetPIDbyName(const char* processName, int intCaseSensitive, int exactMatch)
{
    int pid = -1;
    char commandLine[800];
    char nameOfProcess[800];
    char* stringToCompare = NULL;
    struct dirent* de_dirEntity = NULL;

    DIR* dir_proc = NULL;

    int (*compareFunction) (const char*, const char*, int);

    if(exactMatch)
        compareFunction = &strcmp_ws;
    else
        compareFunction = &strstr_ws;

    if((dir_proc = opendir(PROC_DIRECTORY)) == NULL){
        perror("couldn't open the directory");
        return -1;
    }

    while((de_dirEntity = readdir(dir_proc))){
        if(de_dirEntity->d_type == DT_DIR){
            if(isNumeric(de_dirEntity->d_name)){
                strcpy(commandLine, PROC_DIRECTORY);
                strcat(commandLine, de_dirEntity->d_name);
                strcat(commandLine, "/cmdline");

                FILE* fd_cmdLine = fopen(commandLine, "rt");
                if(fd_cmdLine){
                    fscanf(fd_cmdLine, "%s", nameOfProcess);
                    fclose(fd_cmdLine);

                    if(strrchr(nameOfProcess, '/'))
                        stringToCompare = strrchr(nameOfProcess, '/') + 1;
                    else
                        stringToCompare = nameOfProcess;

                    if(compareFunction(stringToCompare, processName, intCaseSensitive)){
                        pid = atoi(de_dirEntity->d_name);
                        closedir(dir_proc);
                        return pid;
                    }
                        
                }
            }
        }
    }
    closedir(dir_proc);
    return pid;
}

int attach(linuxProc target)
{
    int status;

    if(ptrace(PTRACE_ATTACH, target.procId, NULL, NULL)){
        fprintf(stderr, "error: failed to attach to %d, %s, Try running as root\n", target.procId, strerror(errno));
        return 0;
    }

    if(waitpid(target.procId, &status, 0) ==  -1 || !WIFSTOPPED(status)){
        fprintf(stderr, "error: there was an error waiting for the target to stop.\n");
        fprintf(stdout, "info: %s\n", strerror(errno));
        return 0;
    }

    return -1;
}

int detach(linuxProc target)
{
    return ptrace(PTRACE_DETACH, target.procId, NULL, 0) == 0;
}

int procRead(linuxProc process, int nsize, void* address, void* buffer)
{
    // long data = 0;

    // if((data = ptrace(PTRACE_PEEKDATA, process.procId, address, NULL)) == -1){
    //     perror("read");
    //     return -1;
    // }

    // printf("output from ptrace is %li", data);

    uint8_t data;

    char maps_dir[100];
    FILE* maps_fd;
    long start_addr, end_addr;

    sprintf(maps_dir, "/proc/%d/maps", process.procId);

    if((maps_fd = fopen(maps_dir, "rt")) == NULL){
        perror("open maps file");
        return -1;
    }

    if(fscanf(maps_fd, "%lx-%lx ", &start_addr, &end_addr) == -1){
        perror("read maps file");
        return -1;
    }

    printf("addr %lx\n", start_addr);

    long addr = start_addr;  
    for (; addr < end_addr; ++addr)  
    {  
        data = ptrace(PTRACE_PEEKTEXT, process.procId, addr, NULL);
        printf("data = %x , addr = %lx\n" , data , addr) ;  
    }  
    
    //pread() also work
    //ssize_t pread(int fd, void *buf, size_t count, off_t offset);
    // char file[50];
    // char buf[50];
    // int fd;
    // sprintf(file, "/proc/%ld/mem", (long)process.procId);
    // if((fd = open(file, O_RDWR)) == -1){
    //     printf("file open error\n");
    // }

    // if(pread(fd, buf, sizeof(buf), (off_t)address) == -1){
    //     printf("error read: %m\n");
    // }
    // printf("output from pread is %s", buf);

    // close(fd);

    return 1;
}

linuxProc linuxProcFromID(pid_t pid)
{
    char mem_file_name[1000];
    linuxProc procStruct;

    sprintf(mem_file_name, "/proc/%d/mem", pid);

    procStruct.procId = pid;
    procStruct.procMempath = mem_file_name;

    return procStruct;
}

