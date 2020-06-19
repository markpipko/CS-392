// Mark Pipko and Joshua Mimer
// I pledge my honor that I have abided by the Stevens Honor System.

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];
struct sockaddr_in server_addr;

int handle_stdin() {
    int outcome = get_string(outbuf, MAX_MSG_LEN);
    if(outcome == TOO_LONG){
        fprintf(stderr, "Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
    }
    else if(send(client_socket, outbuf, strlen(outbuf), 0) < 0){
        fprintf(stderr, "Error: Failed to send message to server. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    if(strcmp(outbuf, "bye") == 0){
        printf("Goodbye.\n");
        close(client_socket);
        return EXIT_FAILURE; 
    }
    return EXIT_SUCCESS;
}

int handle_client_socket() {
    //receive data from the socket and store it in inbuf
    ssize_t bytes_received = recv(client_socket, inbuf, BUFLEN, 0);
    if(bytes_received == -1){
        fprintf(stderr, "Warning: Failed to receive incoming message. %s.\n", strerror(errno));
    }
    else if(bytes_received == 0){
        fprintf(stderr, "\nConnection to server has been lost.\n");
        close(client_socket);
        return -1;
    }
    else{
        inbuf[bytes_received] = '\0';
        if(strcmp(inbuf, "bye") == 0){
            printf("\nServer initiated shutdown.\n");
            return -1;
        }
        else{
            printf("\n%s\n", inbuf);
        }
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    int retval = EXIT_SUCCESS;
    
    socklen_t addrlen = sizeof(struct sockaddr_in);
    memset(&server_addr, 0, addrlen); // Zero out structure
 
    // Parse command line argument for server IP and port number.

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Convert character string into a network address.
    int ip_conversion = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    if (ip_conversion == 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'.\n", argv[1]);
        retval = EXIT_FAILURE;
        goto EXIT;
    } else if (ip_conversion < 0) {
        fprintf(stderr, "Error: Failed to convert IP address. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    //Port Check
    int port;   
    if (!parse_int(argv[2], &port, "port number")) {
        return EXIT_FAILURE;
    }
    if (port < 1024 || port > 65535) {
        fprintf(stderr, "Error: Port must be in range [1024, 65535].\n");
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;   // Internet address family
    server_addr.sin_port = htons(port);

    //Username
    int here = 1;
    while(here){
        printf("Enter Username: ");
        fflush(stdout);
        int user = get_string(username, MAX_NAME_LEN + 1);
        if(user == TOO_LONG){
            fprintf(stderr, "Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
            continue;
        }
        if(user == NO_INPUT){
            continue;
        }
        here = 0;
        
    }

    printf("Hello, %s. Let's try to connect to the server.\n", username);

    //Establishing Connection
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    int bytes_read;
    if ((bytes_read = recv(client_socket, inbuf, BUFLEN - 1, 0)) < 0) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    if(bytes_read == 0){
        printf("All connections are busy. Try again later.\n");
    }

    printf("\n");
    printf("%s\n",inbuf);
    printf("\n");
    printf("\n");

    if (send(client_socket, username, strlen(username), 0) < 0) {
        fprintf(stderr, "Error: Failed to send username to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    fd_set socketset;
    int max_socket;
    while (1) {
        printf("[%s]: ", username);
        fflush(stdout);
        FD_ZERO(&socketset); 
        FD_SET(STDIN_FILENO, &socketset);
        FD_SET(client_socket, &socketset); 
        max_socket = client_socket;
        if (select(max_socket+1, &socketset, NULL, NULL, NULL) < 0) {
            fprintf(stderr, "Error: select() failed. %s.\n", strerror(errno));
            retval = EXIT_FAILURE;
            goto EXIT;
        }
        if(FD_ISSET(STDIN_FILENO, &socketset)){
            int handle_outcome = handle_stdin();
            if (handle_outcome == EXIT_FAILURE){
                break;
            }
        }
        else if(FD_ISSET(client_socket, &socketset)){
            int handle_socket = handle_client_socket();
             if (handle_socket == -1){
                break;
            }
        }    
    }
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);    
    }
    return EXIT_SUCCESS;
    EXIT:
    if (fcntl(client_socket, F_GETFD) >= 0) {
        close(client_socket);    
    }
    return retval;
}