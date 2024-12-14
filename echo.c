// file:        echo.c
// purpose:     create a server that echos string inputs
// use:         run server with ./echo_s, which sets up the server
//              connect with telnet (does not work with browser)
//              enter text, server receieves text, server sends back text
//              enter 'quit' to break out of loop, server is still be up and running,
//              ready for more connections
//======================================================================================
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFF_SIZE 1024

int checkForQuit(char* input) {
    char temp[BUFF_SIZE];
    strncpy(temp, input, BUFF_SIZE - 1);
    temp[BUFF_SIZE - 1] = '\0';
    temp[strcspn(temp, "\r\n")] = '\0';
    return strcmp(temp, "quit");
}

void* startServer(void* cfd) {
    int client_fd = *(int*)cfd;
    ssize_t bytes = 0;
    char* buff = (char*)malloc(BUFF_SIZE);
    while(1) { 
        memset(buff, 0, BUFF_SIZE);
        bytes = recv(client_fd, buff, BUFF_SIZE - 1, 0);
        if(bytes < 0) {
            printf("error reading string\n");
            free(buff);
            return NULL;
        }

        if(checkForQuit(buff) == 0) break;

        printf("server received and will echo: %s", buff);
        send(client_fd, buff, strlen(buff), 0);
    }
    free(buff);
    close(client_fd);
    return NULL;
}

int main(int argc, char* argv[]) {
    int port = 80;
    int server_fd, client_fd; 
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    pthread_t tid;

    if(argc > 2) {
        if(strcmp(argv[1], "-p") != 0) {
            printf("invalid flag\n");
            return 1; 
        } else {
            port = atoi(argv[2]);
        }
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0) {
        perror("error creating server\n");
        return 1; 
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if(bind(server_fd, (struct sockaddr*)&addr, addrlen) < 0) {
        perror("error binding\n");
        return 1; 
    }

    if(listen(server_fd, 10) < 0) {
        perror("error listening\n");
        return 1; 
    }

    printf("running echo server on port %d...\n", port);
    printf("type 'quit' to escape\n");
    while(server_fd > 0) {
        client_fd = accept(server_fd, (struct sockaddr*)&addr, &addrlen);
        if(client_fd < 0) {
            perror("error accepting\n");
            return 1; 
        }
        pthread_create(&tid, NULL, startServer, (void*)&client_fd);
        pthread_detach(tid);
    }

    close(server_fd);
    return 0; 
}

