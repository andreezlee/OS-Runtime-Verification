#include <linux/types.h>

typedef struct
{
    char event[10];
    uint64_t param;
} monitor_arg_t;

typedef struct
{
    uint64_t key;
    int state;
    int left;
    int right;
} param_obj_t;

// Functions for a binary search tree implemented with an array

// Returns the index within the array of the target key, -1 if not found
int search_tree(uint64_t key, int next_empty, int curr_index, param_obj_t arr[]);

// Inserts a key into the array, key must not already be inside
void insert_node(uint64_t key, int *next_empty,
    int curr_index, int prev_index, param_obj_t arr[]);