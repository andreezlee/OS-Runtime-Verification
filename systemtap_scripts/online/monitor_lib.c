#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "monitor_lib.h"

// Implementing functions from header
int search_tree(uint64_t key, int next_empty, int curr_index, param_obj_t *arr){
    param_obj_t current_param;
    if (curr_index == -1 || next_empty == 0)
        return -1;
    current_param = arr[curr_index];
    if (current_param.key == key)
        return curr_index;
    if (current_param.key < key)
        return search_tree(key, next_empty, current_param.right, arr);
    if (current_param.key > key)
        return search_tree(key, next_empty, current_param.left, arr);
    return -1;
}

void insert_node(uint64_t key, int *next_empty,
    int curr_index, int prev_index, param_obj_t *arr){
    if (*next_empty >= MAX_NUM_PARAMS){
        printk("SPEC1: SPEC2: SPEC3: Memory capacity exceeded, undefined behavior possible");
        return;
    }
    param_obj_t current_param;
    if (*next_empty == 0)
        goto make_new;
    else if (curr_index == -1){
        if (arr[prev_index].key < key)
            arr[prev_index].right = *next_empty;
        else
            arr[prev_index].left = *next_empty;
        goto make_new;
    }else{
        current_param = arr[curr_index];
        if (current_param.key < key)
            insert_node(key, next_empty, current_param.right, curr_index, arr);
        else
            insert_node(key, next_empty, current_param.left, curr_index, arr);
        return;
    }

make_new:
    arr[*next_empty].key = key;
    arr[*next_empty].state = 0;
    arr[*next_empty].left = -1;
    arr[*next_empty].right = -1;
    int i = *next_empty;
    *next_empty = i + 1;
    return;
}

// Push onto stack
// Precondition: stack cannot be full;
void stack_push(uint64_t *stack, int *stack_head, uint64_t element){
    if (stack[*stack_head] == -1){
        stack[*stack_head] = element;
    }else{
        *stack_head = *stack_head + 1;
        stack[*stack_head] = element;
    }
}

// Pop off of stack
uint64_t stack_pop(uint64_t *stack, int *stack_head){
    if (stack[*stack_head] == -1){
        return -1;
    }else if (*stack_head == 0){
        uint64_t r = stack[*stack_head];
        stack[*stack_head] = -1;
        return r;
    }else{
        *stack_head = *stack_head - 1;
        return stack[*stack_head + 1];
    }
}

EXPORT_SYMBOL(search_tree);
EXPORT_SYMBOL(insert_node);
EXPORT_SYMBOL(stack_push);
EXPORT_SYMBOL(stack_pop);

// Sets up device driver for the ioctl call
static int __init lib_init(void)
{return 0;}

// Removes the device when removed from the system
static void __exit lib_exit(void)
{}

module_init(lib_init);
module_exit(lib_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andre Lee");
MODULE_DESCRIPTION("Monitor Data Structure Library Driver");
