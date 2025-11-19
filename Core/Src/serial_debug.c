//#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>    // STDOUT_FILENO, STDERR_FILENO

#include "main.h"
#include "serial_debug.h"



#ifdef SWO_DEBUG
//overwrite printf() output to send via ITM (SWO)
int _write(int file, char* data, int len) {
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    for (int i = 0; i < len; i++) {
        ITM_SendChar(data[i]);
    }

    // return # of bytes written - as best we can tell
    return len;
}
#endif
