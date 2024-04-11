#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

void print_program_info(char *program_name) {
    printf("\nNAME PROGRAM: %s\n", program_name);
    printf("PID PROGRAM: %d\n", getpid());
    printf("PPID PROGRAM: %d\n", getppid());
}

char* find_variable(const char* name, char* list[]) {
    for (int i = 0; list[i] != NULL; i++) {
      char* var_value = NULL;
        if (strncmp(list[i], name, strlen(name)) == 0)
            return list[i];
    }
    return NULL;
}

void process_file(FILE *file, char *argv[], char *envp[]) {
    char buffer[128];

    while (fgets(buffer, sizeof(buffer), file)) {
        char *new_line = strchr(buffer, '\n');
        if (new_line)
            *new_line = '\0';

        char* value = NULL;
        if (strcmp(argv[2], "+") == 0) {
            value = getenv(buffer);
        } else if (strcmp(argv[2], "*") == 0) {
            value = find_variable(buffer, envp);
        } else if (strcmp(argv[2], "&") == 0) {
            value = find_variable(buffer, environ);
        }

        if (value)
            printf("%s\n", value);
        else
            printf("%s - not found\n", buffer);
    }
}

int main(int argc, char *argv[], char *envp[]) {
    print_program_info(argv[0]);

    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    
    if (!file) {
        printf("File open error\n");
        exit(-1);
    }

    process_file(file, argv, envp);

    fclose(file); 
    
    exit(0);
}
