#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Maximum number of parameters this process is allowed to monitor
#define MAX_PARAMS 50

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
    // Initialize data structures for monitoring,
    // not optimized for efficiency
    char *params[MAX_PARAMS];
    int states[MAX_PARAMS];
    int num_params = 0;

    // Necessary variables to read from file
    FILE * fd;
    char * line = NULL;
    size_t len = 0;
    size_t read;

    fd = fopen("kmalloc.txt", "r");
    if (fd == NULL){
        perror("file could not be opened");
        return -1;
    }

    while ((read = getline(&line, &len, fd)) != -1) {

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
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            case 1:
                if (!strcmp(event, "kmalloc")){
                    states[curr_param] = 4;
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
                    states[curr_param] = 5;
                }else{perror("illegal event");
			        printf("%s\n", event);
                    goto finish_errors;}
                break;
            default: break;
        }
    }

finish_errors:;

    // Gather and print errors, very rudimentary
    int i = 0;
    int errors[4];
    for (i = 0; i < 4; i++){errors[i] = 0;}
    for (i = 0; i < num_params; i++){
        switch(states[i]){
            case 1: errors[0] ++; break;
            case 3: errors[1] ++; break;
            case 4: errors[2] ++; break;
            case 5: errors[3] ++; break;
            default: break;
        }
    }
    printf("Freeing memory without allocation: %d\n", errors[1]);
    printf("Allocated memory but not freed: %d\n", errors[0]);
    printf("Memory allocated twice without being freed: %d\n", errors[2]);
    printf("Memory freed twice without being reallocated: %d\n", errors[3]);

    // Memory cleanup for the program
    for (i = 0; i < num_params; i++){ free(params[i]);}
    fclose(fd);
    free(line);
    return 0;
}
