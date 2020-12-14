#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>

#include "monitor_lib.h"

// Implementing functions from header
int search_tree(uint64_t key, int next_empty, int curr_index, param_obj_t arr[]){
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
    int curr_index, int prev_index, param_obj_t arr[]){
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
    *next_empty++;
    return;
}

EXPORT_SYMBOL(search_tree);
EXPORT_SYMBOL(insert_node);

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
