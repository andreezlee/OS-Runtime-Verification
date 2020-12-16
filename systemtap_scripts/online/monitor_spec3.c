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
#define SPEC "Relaxing symbolic memory location representation: Memory location cannot be reallocated immediately after being freed"

// Functions for a binary search tree implemented with an array
extern int search_tree(uint64_t, int, int, param_obj_t*);
extern void insert_node(uint64_t, int *, int, int, param_obj_t*);
extern void stack_push(uint64_t *, int *, uint64_t);
extern uint64_t stack_pop(uint64_t *, int *);


// Data structures for spec to track
param_obj_t p_array[MAX_NUM_PARAMS];
int next_empty;
uint64_t stack[1];
int stack_head;
int violation_counts;

// Print out violations
static void print_violation(int i) {
    printk("SPEC3: VIOLATION: ");
    violation_counts++;
    switch(i){
        case 1:
            printk("SPEC3: Memory location reallocated immediately after free\n");
            break;
        default:break;
    }
}

void monitor_spec3(monitor_arg_t event){

    char *fun = &(event.event[0]);
    uint64_t key = event.param;

    int curr_state;
    // Reached end of trace, resetting variables
    if (!strcmp(fun, "end")){
        printk("SPEC3: Trace finished, reporting violations and resetting variables");
        printk("SPEC3: Memory location reallocated immediately after free: %d", violation_counts);
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

    // Read the top of the stack
    uint64_t t = stack_pop(stack, &stack_head);

    // State machine simulation
    switch(curr_state){
        case 0:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 1;
                stack_push(stack, &stack_head, t);
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 2;
                stack_push(stack, &stack_head, key);
            }else{ printk("SPEC3: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 1:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 1;
                stack_push(stack, &stack_head, t);
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 2;
                stack_push(stack, &stack_head, key);
            }else{ printk("SPEC3: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        case 2:
            if (!strcmp(fun, "kmalloc")){
                p_array[curr_param].state = 1;
                if (t == key)
                    print_violation(1);
                stack_push(stack, &stack_head, t);
            }else if (!strcmp(fun, "kfree")){
                p_array[curr_param].state = 2;
                stack_push(stack, &stack_head, key);
            }else{ printk("SPEC3: ERROR: wrong event: %s 0x%p\n", fun, (void *)key); }
            break;
        default:break;
    }
}

EXPORT_SYMBOL(monitor_spec3);

// Sets up device driver for the ioctl call
static int __init lib_init(void){
    printk("SPEC3: Initializing Monitor:"); printk("SPEC3: %s", SPEC);
    next_empty = 0; violation_counts = 0; return 0;
}

// Removes the device when removed from the system
static void __exit lib_exit(void){}

module_init(lib_init);
module_exit(lib_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Specification 3 RV Monitor Driver");
