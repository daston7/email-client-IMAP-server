#include "net_utils.h"
#include "mime_handler.h"
#include "retrieve_handler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Function to display MIME-encoded content

void mime_email(int sockfd, int messageNum) {
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE * 10];

    int allResponseCap = BUFFER_SIZE * 10;
    char* allResponse = (char*)malloc(allResponseCap * sizeof(char));
    if (!allResponse) {
        // Error handling if memory allocation fails
        perror("Failed to allocate memory");
        exit(1);
    }
    allResponse[0] = '\0';


    int total_bytes_read = 0;
    int bytes_read;

    if (messageNum == -1) {
        snprintf(command, sizeof(command), "A05 FETCH * (BODY.PEEK[1])\r\n");
    } else {
        snprintf(command, sizeof(command), "A05 FETCH %d (BODY.PEEK[1])\r\n", messageNum);
    }

    send_command(sockfd, command);

    // Read server's response
    while((bytes_read = read_response(sockfd, response)) > 0) {
        // Check whether buffer is available and expand if its not available
        if ((bytes_read + total_bytes_read) >= allResponseCap) {
            char *newBuffer = realloc(allResponse, allResponseCap * 2);
            if (!newBuffer) {
                perror("Failed to reallocate memory");
                free(allResponse);
                exit(2);
            }
            allResponse = newBuffer;
            allResponseCap *= 2;
        }
        // Concatenate to response
        strcat(allResponse, response);


        total_bytes_read += bytes_read;

        // Indicate command has completed
        if (strstr(allResponse, "A05 OK")) {
            break;
        }

    }
    allResponse[total_bytes_read] = '\0';

    int content = extract_number(allResponse);

    int start_printing = 0;
    int first = 1;
    
    // Find starting point for printing content
    for (int i = start_printing; i < total_bytes_read; i++) {
        if (first) {
            if (allResponse[i] == '}') {
                first = 0;
                start_printing = i + 3;
            }
        }
    }

    print_slice(allResponse, start_printing, content);

    free(allResponse);
}