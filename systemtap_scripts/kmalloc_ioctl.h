#ifndef KMALLOC_IOCTL_H
#define KMALLOC_IOCTL_H
#endif
#include <linux/ioctl.h>
 
typedef struct
{
    int size;
    void *ptr;
} kmalloc_arg_t;

#define PERFORM_KMALLOC _IOR('q', 1, kmalloc_arg_t *)
#define PERFORM_KFREE _IOW('q', 3, kmalloc_arg_t *)
#define PERFORM_DUMMY _IOR('q',2, int)