#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include "monitor_module.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int my_open(struct inode *i, struct file *f){return 0;}
static int my_close(struct inode *i, struct file *f){return 0;}

// Tracks last event sent by SystemTap
static int last_event_index = 0;
static monitor_arg_t last_event;
// Tracks last event confirmed by monitor
static int last_confirmed = 0;

// Wait queues for monitor and SystemTap communication
static DECLARE_WAIT_QUEUE_HEAD(sys_wq);
static DECLARE_WAIT_QUEUE_HEAD(mon_wq);

// Ioctl call by monitor will allow communication with SystemTap,
// with this kernel module's global variables as an intermediary.
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
    monitor_arg_t q;
    int new_index;
    switch (cmd)
    {
        case READ_NEW_EVENT:
            copy_from_user(&q, (monitor_arg_t *)arg, sizeof(monitor_arg_t));
            new_index = q.index + 1;
            wait_event_interruptible(mon_wq, last_event_index == new_index);
            if (copy_to_user((monitor_arg_t *)arg, &last_event, sizeof(monitor_arg_t)))
            {
                return -EACCES;
            }
            break;
        case CONFIRMATION:
            copy_from_user(&q, (monitor_arg_t *)arg, sizeof(monitor_arg_t));
            last_confirmed = new_index;
            wake_up_interruptible(&sys_wq);
        default:
            return -EINVAL;
    }

    return 0;
}

int send_to_monitor(monitor_arg_t event){
    int next_confirmed = last_confirmed + 1;
    last_event = event;
    last_event_index = last_event.index;
    wake_up_interruptible(&sys_wq);
    wait_event_interruptible(sys_wq, last_confirmed == next_confirmed);
    return 0;
}

EXPORT_SYMBOL(send_to_monitor);

static struct file_operations monitor_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
    .ioctl = my_ioctl
#else
    .unlocked_ioctl = my_ioctl
#endif
};


// Sets up device driver for the ioctl call
static int __init monitor_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;


    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "rv_monitor")) < 0)
    {
        return ret;
    }

    cdev_init(&c_dev, &monitor_fops);

    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
 
    if (IS_ERR(cl = class_create(THIS_MODULE, "rv_monitor")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "rv_monitor")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }

    return 0;
}

// Removes the device when removed from the system
static void __exit monitor_ioctl_exit(void)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(monitor_ioctl_init);
module_exit(monitor_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Monitor ioctl() Char Driver");
