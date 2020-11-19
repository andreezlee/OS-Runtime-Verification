#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/sched.h>

#include "kmalloc_ioctl.h"

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static int my_open(struct inode *i, struct file *f){return 0;}
static int my_close(struct inode *i, struct file *f){return 0;}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
    kmalloc_arg_t q;
    switch (cmd)
    {
        case PERFORM_KMALLOC:
            printk("The process name is %s\n", current->comm);
            copy_from_user(&q, (kmalloc_arg_t *)arg, sizeof(kmalloc_arg_t));
            q.ptr = kmalloc(q.size, GFP_USER);

            if (!q.ptr || copy_to_user((kmalloc_arg_t *)arg, &q, sizeof(kmalloc_arg_t)))
            {
                return -EACCES;
            }
            break;
        case PERFORM_KFREE:
            if (copy_from_user(&q, (kmalloc_arg_t *)arg, sizeof(kmalloc_arg_t)))
            {
                return -EACCES;
            }
            kfree(q.ptr);
            break;
        case PERFORM_DUMMY:
            printk(KERN_INFO "Dummy: Hello!!!\n");
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

static struct file_operations kmalloc_fops =
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

static int __init kmalloc_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;


    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "kmalloc")) < 0)
    {
        return ret;
    }

    cdev_init(&c_dev, &kmalloc_fops);

    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
 
    if (IS_ERR(cl = class_create(THIS_MODULE, "kmalloc")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "kmalloc")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }

    return 0;
}

static void __exit kmalloc_ioctl_exit(void)
{
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(kmalloc_ioctl_init);
module_exit(kmalloc_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Kmalloc ioctl() Char Driver");
