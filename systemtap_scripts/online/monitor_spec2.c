#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include "monitor_lib.h"

// Specification-specific info for users
#define SPEC "Approximation of symbolic memory location representation: Memory location cannot be reallocated after being freed"

// Functions for a binary search tree implemented with an array
extern int search_tree(uint64_t, int, int, param_obj_t*);
extern void insert_node(uint64_t, int *, int, int, param_obj_t*);

// Data structures for spec to track
param_obj_t p_array[MAX_NUM_PARAMS];
int next_empty;
int violation_counts;

// Print out violations
void print_violation(int i) {
    printk("SPEC2: VIOLATION: ");
    violation_counts++;
    switch(i){
        case 1:
            printk("SPEC2: Memory location reallocated after free\n");
            break;
        default:break;
    }
}

void monitor_spec2(monitor_arg_t event){

    char *fun = &(event.event[0]);
    uint64_t key = event.param;

    int curr_state;
    // Reached end of trace, resetting variables
    if (!strcmp(fun, "end")){
        printk("SPEC2: Trace finished, reporting violations and resetting variables");
        printk("SPEC2: Memory location reallocated after free: %d", violation_counts);
        next_empty = 0;
        violation_counts = 0;
        return;
    }

    int curr_param = search_tree(key, next_empty, 0, p_array);
    if (curr_param < 0) {
        curr_param = next_empty;
        insert_node(key, &next_empty, 0, 0, p_array);
        curr_state = 0;
    }else{
        curr_state = p_array[curr_param].state;
    }

    // State machine simulation
    switch(curr_state){
        case 0:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 1;
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 3;
            }else{ printk("SPEC2: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 1:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 3;
                print_violation(1);
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 2;
            }else{ printk("SPEC2: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 2:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 3;
                print_violation(1);
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 2;
            }else{ printk("SPEC2: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 3:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 3;
                print_violation(1);
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 2;
            }else{ printk("SPEC2: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        default:break;
    }
}

EXPORT_SYMBOL(monitor_spec2);

// Sets up device driver for the ioctl call
static int __init lib_init(void){
    printk("SPEC2: Initializing Monitor:"); printk("SPEC2: %s", SPEC);
    next_empty = 0; violation_counts = 0; return 0;
}

// Removes the device when removed from the system
static void __exit lib_exit(void){}

module_init(lib_init);
module_exit(lib_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Specification 2 RV Monitor Driver");
