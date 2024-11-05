// Author:      Angel Penaloza
// Assignment:  Echo Server (with multithreading)
// Description: Creating a server which echos all messages received, line by line.
//              Implemented with the echo_server funciton. 
//              Funciton returns a void pointer data type, to accommodate for the multithreading.
//              Takes one integer argument, which is the user specified port number to connect to.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

//---- echo server function ----//
void* echo_server(void* v_port) {
    printf("enter\n");

    // variables
    int* p = (int*)v_port;
    int port = *p;
    int server_file_desc;
    int new_socket = 1;
    int opt = 1; 
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    char buffer[100] = {0};

    // create file descriptor
    server_file_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(server_file_desc < 0) {
        perror("error creating socket\n");
        return NULL;
    }

    // set port to given port
    if(setsockopt(server_file_desc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("error setting the sock opt\n");
        return NULL;
    } 

    // set addr values
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    // bind
    if(bind(server_file_desc, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("error binding\n");
        return NULL;
    }

    // listen
    if(listen(server_file_desc, 3) < 0) {
        perror("error listening\n");
        return  NULL;
    }

    // create new socket using accept function
    new_socket = accept(server_file_desc, (struct sockaddr*)&addr, &addrlen);
    if(new_socket < 0) {
        perror("error accepting\n");
        return NULL;
    }

    // read and print in loop
    while(new_socket > 0) {
        memset(buffer, 0, sizeof(buffer));
        read(new_socket, buffer, 100 - 1);
        printf("%s", buffer);
    }

    // close both sockets
    close(new_socket);
    close(server_file_desc);
    
    return NULL;
}
//---- end of echo server function ----//

int main(int argc, char* argv[]) {

    // make sure there is a valid amount of arguments
    if(argc < 2) {
        printf("insufficient arguments\n");
        exit(1);
    }

    // make sure the right flag was sent
    if(strcmp(argv[1], "-p") != 0) {
        printf("invalid flag\n");
        exit(1);
    }

    // variables
    int port = atoi(argv[2]);
    pthread_t tid; 


    // run the echo server
    pthread_create(&tid, NULL, echo_server, (void*)&port);
    pthread_join(tid, NULL);

    return 0;
}