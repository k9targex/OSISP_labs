#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>

#define BUFFER_SIZE 2048

char* hello_string;

char* read_hello(char* fname) 
{
    char* temp;

    FILE* f = fopen(fname, "r");
    if(f == NULL) {
        printf("Error while open file.\n");
        exit(0);
    }

    temp = (char*)malloc(255);

    fgets(temp, 255, f);

    fclose(f);

    temp[strlen(temp)] = '\0';
    return temp;
}

void get_current_time(char* buffer, size_t len) 
{
    struct timespec ts;
    struct tm *tm_info;

    clock_gettime(CLOCK_REALTIME, &ts);

    tm_info = localtime(&ts.tv_sec);

    strftime(buffer, len, "[%Y.%m.%d-%H:%M:%S]", tm_info);
}

void echo(int client_socket, const char* buffer) 
{
    char response[BUFFER_SIZE];
    char timestamp[100];
    
    get_current_time(timestamp, sizeof(timestamp));

    snprintf(response, sizeof(response), "%s %s", timestamp, buffer + 5);

    write(client_socket, response, strlen(response));
}

void quit(int client_socket) 
{
    char response[BUFFER_SIZE];
    char timestamp[100];

    get_current_time(timestamp, sizeof(timestamp));
    snprintf(response, sizeof(response), "%s BYE", timestamp);

    write(client_socket, response, strlen(response));

    close(client_socket);
}

void info(int client_socket) 
{
    char response[BUFFER_SIZE];
    char timestamp[100];

    get_current_time(timestamp, sizeof(timestamp));

    snprintf(response, sizeof(response), "%s %s", timestamp, hello_string);

    write(client_socket, response, strlen(response));
}

void cd(int client_socket, char *current_dir, const char* buffer, const char* root_dir) 
{
    char response[BUFFER_SIZE * 2];
    char timestamp[100];

    get_current_time(timestamp, sizeof(timestamp));
    char new_dir[BUFFER_SIZE];

    snprintf(new_dir, sizeof(new_dir), "%s/%s", current_dir, buffer + 3);
    if (new_dir[strlen(new_dir) - 1] == '\n')
        new_dir[strlen(new_dir) - 1] = '\0';

    if (strncmp(buffer + 3, "..", 2) == 0) 
    {
        if (strcmp(current_dir, root_dir) == 0) 
            snprintf(response, sizeof(response), "%s Root!", timestamp);
        else 
        {
            if (chdir("..") == 0) 
            {
                getcwd(current_dir, BUFFER_SIZE);
                if (strncmp(current_dir, root_dir, strlen(root_dir)) != 0) {

                    chdir(root_dir);
                    getcwd(current_dir, BUFFER_SIZE);
                    snprintf(response, sizeof(response), "%s Access denied", timestamp);
                } 
                else 
                {
                    snprintf(response, sizeof(response), "%s Changed to: %s", timestamp, current_dir);
                }
            } 
            else 
            {
                snprintf(response, sizeof(response), "%s Cannot change directory", timestamp);
            }
        }
    } 
    else 
    {
        if (chdir(new_dir) == 0) 
        {
            getcwd(current_dir, BUFFER_SIZE);

            if (strncmp(current_dir, root_dir, strlen(root_dir)) != 0) 
            {
                chdir(root_dir);
                getcwd(current_dir, BUFFER_SIZE);
                snprintf(response, sizeof(response), "%s Access denied", timestamp);
            } 
            else 
            {
                snprintf(response, sizeof(response), "%s Changed to: %s", timestamp, new_dir);
            }
        } 
        else 
        {
            snprintf(response, sizeof(response), "%s No such dir", timestamp);
        }
    }
    write(client_socket, response, strlen(response));
}

void list(int client_socket, char *current_dir) 
{
    char response[BUFFER_SIZE];
    char timestamp[100];

    get_current_time(timestamp, sizeof(timestamp));

    DIR *dir;
    struct dirent *entry;

    dir = opendir(current_dir);

    if (dir == NULL)
        snprintf(response, sizeof(response), "%s Cant list directory", timestamp); 
    else 
    {
        snprintf(response, sizeof(response), "%s\n", timestamp);

        while ((entry = readdir(dir)) != NULL) 
        {
            char full_path[PATH_MAX];  
            snprintf(full_path, sizeof(full_path), "%s/%s", current_dir, entry->d_name);
            
            if (entry->d_type == DT_DIR) 
            {
                strncat(response, entry->d_name, sizeof(response) - strlen(response) - 1);
                strncat(response, "/", sizeof(response) - strlen(response) - 1);
            } 
            else if (entry->d_type == DT_REG) 
                strncat(response, entry->d_name, sizeof(response) - strlen(response) - 1);
            else if (entry->d_type == DT_LNK) 
            {
                char link_target[PATH_MAX];  
                ssize_t len = readlink(full_path, link_target, sizeof(link_target) - 1);

                if (len != -1) 
                {
                    link_target[len] = '\0';
                    char link_info[NAME_MAX + PATH_MAX + 10]; 

                    if (lstat(link_target, &(struct stat){0}) == 0 && S_ISLNK(((struct stat){0}).st_mode))
                        snprintf(link_info, sizeof(link_info), "%s -->> %s", entry->d_name, link_target);
                    else
                        snprintf(link_info, sizeof(link_info), "%s --> %s", entry->d_name, link_target);
                    strncat(response, link_info, sizeof(response) - strlen(response) - 1);
                } 
                else
                    strncat(response, entry->d_name, sizeof(response) - strlen(response) - 1);
            }
            strncat(response, "\n", sizeof(response) - strlen(response) - 1);
        }
        closedir(dir);
    }
    write(client_socket, response, strlen(response) - 1);
}

void unknown_command(int client_socket) 
{
    char response[BUFFER_SIZE];
    char timestamp[100];

    get_current_time(timestamp, sizeof(timestamp));

    snprintf(response, sizeof(response), "%s UNKNOWN", timestamp);

    write(client_socket, response, strlen(response));
}

void* client(void *arg) 
{
    char *root_dir = ((char **)arg)[0];
    int client_socket = *(int *)((char **)arg)[1];
    free(arg);
    
    char buffer[BUFFER_SIZE];
    char current_dir[BUFFER_SIZE];
    strcpy(current_dir, root_dir);

    while (1) 
    {
        bzero(buffer, BUFFER_SIZE);
        ssize_t nread = read(client_socket, buffer, BUFFER_SIZE - 1);
        if (nread <= 0) 
        {
            if (nread == 0)
                printf("%s", "Client disconnected.\n");
            else 
                perror("Read failed");
            close(client_socket);
            return NULL;
        }

        if (strncmp(buffer, "ECHO ", 5) == 0)
            echo(client_socket, buffer);
        else if (strncmp(buffer, "QUIT", 4) == 0) 
        {
            quit(client_socket);
            return NULL;
        } 
        else if (strncmp(buffer, "INFO", 4) == 0) 
            info(client_socket);
        else if (strncmp(buffer, "CD ", 3) == 0)
            cd(client_socket, current_dir, buffer, root_dir);
        else if (strncmp(buffer, "LIST", 4) == 0)
            list(client_socket, current_dir);
        else 
            unknown_command(client_socket);
    }
}

int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        perror("Not enough args!");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // port
    char* root_dir = realpath(argv[2], NULL); // directory

    hello_string = read_hello(argv[3]); // file

    if (root_dir == NULL) 
    {
        perror("Invalid root directory");
        exit(EXIT_FAILURE);
    }

    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) 
    {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) 
    {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    if (listen(serverfd, 20) == -1) 
    {
        perror("Listen error");
        exit(EXIT_FAILURE);
    }

    printf("READY TO USE.\n");

    while (1) 
    {
        socklen_t addrlen = sizeof(addr);
        int clientfd = accept(serverfd, (struct sockaddr*)&addr, &addrlen);
        
        if (clientfd == -1) 
        {
            perror("accept error");
            exit(EXIT_FAILURE);
        }

        pthread_t thread_id;
        int *new_sock = malloc(sizeof(int));
        *new_sock = clientfd;

        char **args = malloc(2 * sizeof(char *));
        args[0] = root_dir;
        args[1] = (char *)new_sock;

        if (pthread_create(&thread_id, NULL, client, (void *)args) < 0) 
        {
            perror("pthread_create");
            free(new_sock);
            free(args);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(serverfd);

    free(root_dir);

    return 0;
}