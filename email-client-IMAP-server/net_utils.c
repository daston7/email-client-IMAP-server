#include "net_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

// Print specific slice of a string
void print_slice(const char *response, int start, int length) {
    printf("%.*s", length, response + start);
}

// Function to create socket and connect it to a server
int create_socket(const char *hostname, int port, int use_tls) {
    struct addrinfo hints, *res, *res0;
    int sockfd, err;
    char portstr[6];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    snprintf(portstr, sizeof(portstr), "%d", port);

    err = getaddrinfo(hostname, portstr, &hints, &res0);
    if (err) {
        printf("Failed to resolve hostname\n");
        exit(2);
    }
    // Loop through all the results and connect to the first one
    for (res = res0; res; res = res->ai_next) {
        sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sockfd < 0)
            continue;

        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
            break;

        close(sockfd);
    }

    freeaddrinfo(res0);

    // Handle if connection was successful
    if (!res){
        printf("Could not connect to server\n");
        exit(2);
    }

    return sockfd;
}

// Function to send command to server
void send_command(int sockfd, const char *command) {
    if (send(sockfd, command, strlen(command), 0) < 0) {
        printf("Failed to send command to server\n");
        exit(3);
    }
}

// Function to read response from server to buffer
int read_response(int sockfd, char *buffer) {
    int n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (n < 0) {
        printf("Error reading from socket\n");
        exit(3);
    }
    buffer[n] = '\0';
    return n;
}

