#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

size_t s = 50;

int main(void)
{
    // Create a hash table to store state data for parameters
    if (!hcreate(s)){
        perror("table creation failed");
        return -1;
    }

    // Necessary variables to read from file
    FILE * fd;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fd = fopen("kmalloc.txt", "r");
    if (fd == NULL){
        perror("file could not be opened");
        hdestroy();
        return -1;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Full line: %s\n", line);
        char *event = strtok(line, " ");
        char *param = strtok(NULL, " ");
        printf("Event: %s\n", event);
        printf("Parameter: %s\n", param);
    }

    hdestroy();
    fclose(fd);
    free(line);
    return 0;
}