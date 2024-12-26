#include "imap_utils.h"
#include "net_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Function to login to serv er
void login_to_imap(int sockfd, const char *username, const char *password) {
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Read responses from server
    read_response(sockfd, response);


    snprintf(command, sizeof(command), "A01 LOGIN %s %s\r\n", username, password);
    send_command(sockfd, command);
    read_response(sockfd, response);

    // Check if logic is successful
    if (!strstr(response, "A01 OK")) {
        printf("Login failure\n");
        // Exit if fails
        exit(3);
    }
}

// Function to select folder from server
void select_folder(int sockfd, const char *folder) {
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];


    snprintf(command, sizeof(command), "A02 SELECT \"%s\"\r\n", folder);
    send_command(sockfd, command);
    read_response(sockfd, response);

    // Check if selection of folder is successful or not
    if (!strstr(response, "A02 OK")) {
        printf("Folder not found\n");
        exit(3);
    }
}