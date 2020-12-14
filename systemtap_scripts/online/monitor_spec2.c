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

// Print out violations
void print_violation(int i) {
    printf("SPEC2: VIOLATION: ");
    switch(i){
        case 1:
            printf("SPEC2: Memory location reallocated after free\n");
            break;
        default:break;
    }
}


// Functions for a binary search tree implemented with an array
extern int search_tree(uint64_t, int, int, param_obj_t*);

extern void insert_node(uint64_t, int *, int, int, param_obj_t*);


// Data structures for spec to track
param_obj_t p_array[10000000];
int next_empty;

void monitor_spec2(monitor_arg_t event){

    char *fun = &(event.event[0]);
    uint64_t key = event.param;

    printk("SPEC2: Received event: %s 0x%p", fun, (void *)key);

    int curr_state;
    // Reached end of trace, resetting variables
    if (!strcmp(fun, "end")){
        printk("SPEC2: Trace finished, resetting variables");
        for (curr_state = 0; curr_state < next_empty; curr_state++){
            printk("SPEC2: Checking node %d: key 0x%llu, state %d, left %d, right %d", curr_state,
                p_array[curr_state].key, p_array[curr_state].state, p_array[curr_state].left, p_array[curr_state].right);
        }
        next_empty = 0;
        return;
    }

    int curr_param = search_tree(key, next_empty, 0, p_array);
    if (curr_param < 0) {
        printk("SPEC2: 0x%p not found, adding into insert_node", (void *)key);
        curr_param = next_empty;
        insert_node(key, &next_empty, 0, 0, p_array);
        curr_state = 0;
    }else{
        printk("SPEC2: 0x%p found, taking existing state", (void *)key);
        curr_state = p_array[curr_param].state;
    }

    printk("SPEC2: current state: %d\n", curr_state);
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
    printk("SPEC2: new state is now %d", p_array[curr_param].state);
}

EXPORT_SYMBOL(monitor_spec2);

// Sets up device driver for the ioctl call
static int __init lib_init(void){
    printk("SPEC2: Initializing Monitor:"); printk("SPEC2: %s", SPEC);
    next_empty = 0; return 0;
}

// Removes the device when removed from the system
static void __exit lib_exit(void){}

module_init(lib_init);
module_exit(lib_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Specification 2 RV Monitor Driver");
