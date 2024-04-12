#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>

#define CHILD_PATH         "CHILD_PATH"
#define PROGRAMM_NAME_LEN  10
#define MAX_PROCESSES      90

extern char** environ;

int compare_strings(const void *a, const void *b) 
{
    return strcmp(*(const char **)a, *(const char **)b);
}

char* get_env_value(const char* env_name, char* envp[]) {
    char* value = NULL;
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp(envp[i], env_name, strlen(env_name)) == 0) {
            value = strchr(envp[i], '=');
            if (value != NULL)
                value++;
            break;
        }
    }
    return value;
}

void execute_child( char* var_value, char* file_name,  char *choice, char* envp[],int *count) {
    pid_t pid = fork();
    if (pid == 0) {
        char child_programm_name[PROGRAMM_NAME_LEN];

        snprintf(child_programm_name, sizeof(child_programm_name), "child_%02d", *count);

        char* new_argv[] = {child_programm_name,file_name, choice,NULL};
        
                        // char* new_argv[] = {child_programm_name, argv[1], "+", NULL};

        execve(var_value, new_argv, envp);
    }
    else if (pid > 0)
        (*count)++;

}

int main(int argc, char* argv[], char* envp[])
{
    setlocale(LC_COLLATE, "C");

    int env_count = 0;
    char** env_vars = NULL;
    while (environ[env_count] != NULL) 
    {
        env_vars = (char**)realloc(env_vars, (env_count + 1) * sizeof(char*));
        env_vars[env_count] = environ[env_count];
        env_count++;
    }

    qsort(env_vars, env_count, sizeof(char*), compare_strings);

    for (int i = 0; i < env_count; i++)
        printf("%s\n", env_vars[i]);

    free(env_vars);

    int child_status;
    int count = 0;
    while (true)
    {
        char choice = getchar();

        if (count < MAX_PROCESSES)
        {
            switch (choice)
            {
                case '+':
                {
                    char* var_value = getenv(CHILD_PATH);
                    if (var_value != NULL)
                        execute_child(var_value, argv[1], "+", envp,&count);
                    else
                        printf("\n'%s' environment variable not found\n", CHILD_PATH);
                    break;
                }
                case '*':
                {
                    char* var_value = get_env_value(CHILD_PATH, envp);
                    if (var_value != NULL)
                        execute_child(var_value, argv[1], "*", envp,&count);
                    else
                        printf("\n'%s' environment variable not found\n", CHILD_PATH);
                    break;
                }
                case '&':
                {
                    char* var_value = get_env_value(CHILD_PATH, environ);
                    if (var_value != NULL)
                        execute_child(var_value, argv[1], "&", environ,&count);
                    else
                        printf("\n'%s' environment variable not found\n", CHILD_PATH);
                    break;
                }
                case 'q':
                {
                    return 0;
                }
            }
            wait(&child_status);
            if (child_status != 0)
                printf("\nChild execute error!\n");   
        }
        else
            printf("\nProcesses is now higher than 90\n");

    }
    return 0;
}
