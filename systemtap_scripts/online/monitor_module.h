#ifndef KMALLOC_IOCTL_H
#define KMALLOC_IOCTL_H
#endif
#include <linux/ioctl.h>
 
typedef struct
{
    char event[2];
    void *ptr;
    int index;
} monitor_arg_t;

// Defining these in a header function allow it to be
// used by only user space programs that include it.
#define READ_NEW_EVENT _IOR('!', 10, monitor_arg_t *)
#define CONFIRMATION _IOR('!',20, monitor_arg_t *)