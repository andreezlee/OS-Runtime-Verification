#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Example Linux module");
MODULE_VERSION("0.01");


typedef struct
{
	char name[64];
}test_struct;


static int __init example_init(void){
	printk(KERN_INFO "Hello, World!\n");
	printk("The process name is %s\n", current->comm);
	printk("The process tty is %d\n", current->signal->tty);
	printk("The process group is %d\n", (int) task_tgid_nr(current));
	printk("The struct size is %d\n", sizeof(test_struct));
	printk("\n\n");

	return 0;

}



static void __exit example_exit(void){
	printk(KERN_INFO "Goodbye, World!\n");
}

module_init(example_init);
module_exit(example_exit);