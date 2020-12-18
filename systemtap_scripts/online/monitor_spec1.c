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
#define SPEC "Allocated memory must be freed before it can be reallocated, and memory cannot be freed without being allocated"

// Functions for a binary search tree implemented with an array
extern int search_tree(uint64_t, int, int, param_obj_t*);

extern void insert_node(uint64_t, int *, int, int, param_obj_t*);


// Data structures for spec to track
param_obj_t p_array[MAX_NUM_PARAMS];
int next_empty;
int violation_counts[4];

// Print out violations
static void print_violation(int i) {
    printk("SPEC1: VIOLATION: ");
    violation_counts[i]++;
    switch(i){
        case 0:
            printk("SPEC1: Memory allocated but not freed\n");
            break;
        case 1:
            printk("SPEC1: Freeing memory without allocation\n");
            break;
        case 2:
            printk("SPEC1: Memory allocated multiple times without being freed\n");
            break;
        case 3:
            printk("SPEC1: Memory freed multiple times without being reallocated\n");
            break;
        default:break;
    }
}

void monitor_spec1(monitor_arg_t event){

    char *fun = &(event.event[0]);
    uint64_t key = event.param;

    int curr_state;
    // Reached end of trace, resetting variables
    if (!strcmp(fun, "end")){
        printk("SPEC1: Trace finished. Resetting variables, finding more violations");
        for (curr_state = 0; curr_state < next_empty; curr_state++){
            if (p_array[curr_state].state == 1)
                print_violation(0);
        }
        printk("SPEC1: Memory allocated but not freed: %d", violation_counts[0]);
        printk("SPEC1: Freeing memory without allocation: %d", violation_counts[1]);
        printk("SPEC1: Memory allocated multiple times without being freed: %d", violation_counts[2]);
        printk("SPEC1: Memory freed multiple times without being reallocated: %d", violation_counts[3]);
        for (curr_state = 0; curr_state < 4; curr_state++)
            violation_counts[curr_state] = 0;
        next_empty = 0;
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
                print_violation(1);
            }else{ printk("SPEC1: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 1:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 1;
                print_violation(2);
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 2;
            }else{ printk("SPEC1: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 2:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 1;
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 3;
                print_violation(3);
            }else{ printk("SPEC1: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 3:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 1;
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 3;
                print_violation(3);
            }else{ printk("SPEC1: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        default:break;
    }
}

EXPORT_SYMBOL(monitor_spec1);

// Sets up device driver for the ioctl call
static int __init lib_init(void){
    printk("SPEC1: Initializing Monitor:"); printk("SPEC1: %s", SPEC);
    next_empty = 0; int i;
    for (i = 0; i < 4; i++)
        violation_counts[i] = 0;
    return 0;
}

// Removes the device when removed from the system
static void __exit lib_exit(void){}

module_init(lib_init);
module_exit(lib_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Specification 1 RV Monitor Driver");
