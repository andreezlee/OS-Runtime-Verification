#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPEC "Allocated memory must be freed before it can be reallocated, and memory cannot be freed without being allocated"

// Maximum number of parameters this process is allowed to monitor
#define MAX_PARAMS 50

// Print out violations
void print_violation(int i, int event) {
	printf("VIOLATION: ");
    switch(i){
        case 1:
            printf("Freeing memory without allocation: event %d\n", event);
            break;
        case 2:
            printf("Memory allocated multiple times without being freed: event %d\n", event);
            break;
        case 3:
            printf("Memory freed multiple times without being reallocated: event %d\n", event);
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

int main(void)
{
	// Printing specification
	printf("\nSTARTING MONITOR FOR SPECIFICATION:\n");
	printf("%s\n\n", SPEC);
    // Initialize data structures for monitoring,
    // not optimized for efficiency
    int i = 0;
    int violations[4];
    for (i = 0; i < 4; i++){violations[i] = 0;}
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

        // State machine simulation
        switch(curr_state){
            case 0:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 1;
                }else if (!strcmp(event, "kfree")){
                    states[curr_param] = 3;
                    print_violation(1,lineNum);
                    violations[1]++;
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            case 1:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 1;
                    print_violation(2, lineNum);
                    violations[2]++;
                }else if (!strcmp(event, "kfree")){
                    states[curr_param] = 2;
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            case 2:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 1;
                }else if (!strcmp(event, "kfree")){
                    states[curr_param] = 3;
                    print_violation(3, lineNum);
                    violations[3]++;
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            case 3:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 1;
                }else if (!strcmp(event, "kfree")){
                    states[curr_param] = 3;
                    print_violation(3, lineNum);
                    violations[3]++;
                }else{perror("illegal event");
                    printf("%s\n", event);
                    goto finish_errors;}
                break;
            default:break;
        }
    }

finish_errors:;

    // Gather and print errors, very rudimentary
    for (i = 0; i < num_params; i++){
        switch(states[i]){
            case 1: violations[0] ++; break;
            default:break;
        }
    }
    printf("\nTOTAL VIOLATIONS:\n");
    printf("Allocated memory but not freed: %d\n", violations[0]);
    printf("Freeing memory without allocation: %d\n", violations[1]);
    printf("Memory allocated twice without being freed: %d\n", violations[2]);
    printf("Memory freed twice without being reallocated: %d\n", violations[3]);

    // Memory cleanup for the program
    for (i = 0; i < num_params; i++){ free(params[i]);}
    fclose(fd);
    free(line);
    return 0;
}
