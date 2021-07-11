#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <sys/uio.h>

int main(void)
{
    struct iovec local[2];
    struct iovec remote[1];
    char buf1[10];
    char buf2[10];
    ssize_t nread;
    pid_t pid = 23126;             /* PID of remote process */

    local[0].iov_base = buf1;
    local[0].iov_len = 10;
    local[1].iov_base = buf2;
    local[1].iov_len = 10;
    remote[0].iov_base = (void *) 0x8b6b3000;
    remote[0].iov_len = 20;

    nread = process_vm_readv(pid, local, 2, remote, 1, 0);
    if (nread != 20){
        fprintf(stderr, "read error: %m\n");
        return 1;
    }
    else
        return 0;
}
