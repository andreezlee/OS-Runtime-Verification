#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

extern int GLOBAL_DUMMY;
extern void print_hello(int);
extern void print_goodbye(int);


static int __init dummy_ioctl_init(void){
	int a = GLOBAL_DUMMY;
	print_hello(a);
	return 0;
}

static void __exit dummy_ioctl_exit(void){
	int a = GLOBAL_DUMMY;
	print_goodbye(a);
}


module_init(dummy_ioctl_init);
module_exit(dummy_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Dummy Driver");