#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPEC "Relaxing symbolic memory location representation: Memory location cannot be reallocated immediately after being freed"

// Maximum number of parameters this process is allowed to monitor
#define MAX_PARAMS 100000

// Maximum stack size needed for the state machine
#define MAX_STACK_SIZE 1

// Print out violations
void print_violation(int i, int event) {
    printf("VIOLATION: ");
    switch(i){
        case 1:
            printf("Memory location reallocated immediately after free: event %d\n", event);
            break;
        default:break;
    }
}

// Search for parameter p
int search_state(char *p, char *params[], int num_params){
    int i;
    for (i = 0; i < num_params; i++){
        if (strcmp(p, params[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// Push onto stack
// Precondition: stack cannot be full;
void push(char *stack[], int *stack_head, char *element){
    if (!strcmp(stack[*stack_head], "empty")){
        stack[*stack_head] = element;
    }else{
        *stack_head++;
        stack[*stack_head] = element;
    }
}

// Pop off of stack
char* pop(char * stack[], int *stack_head){
    if (!strcmp(stack[*stack_head], "empty")){
        return "empty";
    }else if (*stack_head == 0){
        char * r = stack[*stack_head];
        stack[*stack_head] = "empty";
        return r;
    }else{
        *stack_head--;
        return stack[*stack_head + 1];
    }
}

int main(void)
{
	// Printing specification
    printf("\nSTARTING MONITOR FOR SPECIFICATION:\n");
    printf("%s\n\n", SPEC);
    // Initialize data structures for monitoring,
    // not optimized for efficiency
    int i = 0;
    int violations[1];
    for (i = 0; i < 1; i++){violations[i] = 0;}
    char *stack[MAX_STACK_SIZE];
    int stack_head = 0;
    stack[stack_head] = "empty"; // string "empty" is treated as empty stack in these monitors
    char *params[MAX_PARAMS];
    int states[MAX_PARAMS];
    int num_params = 0;

    // Necessary variables to read from file
    FILE * fd;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    int lineNum = 0;

    fd = fopen("kmalloc.txt", "r");
    if (fd == NULL){
        perror("file could not be opened");
        return -1;
    }

    while ((read = getline(&line, &len, fd)) != -1) {
        lineNum++;

        // Parse line for event and parameter
        char *event = strtok(line, " ");
        if (!strcmp(event, "end")){
            goto finish_errors;
        }
        char *p = strtok(NULL, " ");

        // Search for previous state of parameter, if not found
        // set the previous state to the initial state instead.
        int curr_param = search_state(p, params, num_params);
        int curr_state = 0;
        if (curr_param < 0) {
            // Add new parameter to the monitoring
            num_params ++;
            curr_param = num_params - 1;
            params[curr_param] = (char *)(malloc(strlen(p) + 1));
            if (strcpy(params[curr_param], p) != params[curr_param]){
                perror("Kernel has failed the call");
                goto finish_errors;
            }
        }else{
            curr_state = states[curr_param];
        }

        // Read the top of the stack
        char * t = pop(stack, &stack_head);

        // State machine simulation
        switch(curr_state){
            case 0:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 1;
                    push(stack, &stack_head, t);
                }else if (!strcmp(event, "kfree")){
                    states[curr_param] = 2;
                    push(stack, &stack_head, params[curr_param]);
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            case 1:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 1;
                    push(stack, &stack_head, t);
                }else if (!strcmp(event, "kfree")){
                    states[curr_param] = 2;
                    push(stack, &stack_head, params[curr_param]);
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            case 2:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 1;
                    push(stack, &stack_head, t);
                    if (!strcmp(p, t)){
                        print_violation(1, lineNum);
                        violations[0]++;
                    }
                }else if (!strcmp(event, "kfree")){
                    states[curr_param] = 2;
                    push(stack, &stack_head, params[curr_param]);
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            default:break;
        }

    }

finish_errors:;

    printf("\nTOTAL VIOLATIONS:\n");
    printf("Memory location reallocated immediately after free: %d\n", violations[0]);
    // Memory cleanup for the program
    for (i = 0; i < num_params; i++){ free(params[i]);}
    fclose(fd);
    free(line);
    return 0;
}
