#include <stdio.h>
#include "net_utils.h"
#include <string.h>
#include <stdlib.h>

// Extract first number after the curly bracket
int extract_number(const char *str) {
    int number = 0;
    const char *start = strchr(str, '{');
    if (start) {
        sscanf(start, "{%d", &number);
    }
    return number;
}

// Fetches email rom server with specific message number
void retrieve_email(int sockfd, int messageNum) {
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE * 10];

    int allResponseCap = BUFFER_SIZE * 10;
    char* allResponse = (char*)malloc(allResponseCap * sizeof(char));
    if (!allResponse) {
        perror("Failed to allocate memory");
        exit(1);
    }
    allResponse[0] = '\0';


    int total_bytes_read = 0;
    int bytes_read;

    if (messageNum == -1) { 
        strcpy(command, "A03 FETCH * (BODY.PEEK[])\r\n");
    } else {
        snprintf(command, sizeof(command), "A03 FETCH %d (BODY.PEEK[])\r\n", messageNum);
    }
    send_command(sockfd, command);

    // Read response until completion
    while((bytes_read = read_response(sockfd, response)) > 0) {
        // Check and adjust buffer size
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
        // Concat to response buffer
        strcat(allResponse, response);


        total_bytes_read += bytes_read;

        if (strstr(allResponse, "A03 OK")) {
            break;
        }
        if (strstr(allResponse, "A03 BAD")) {
            printf("Message not found\n");
            free(allResponse);
            exit(3);
        }

    }
    allResponse[total_bytes_read] = '\0';

    // Extract content from response
    int content = extract_number(allResponse);

    int start_printing = 0;
    int first = 1;

    // Find start index to print email content
    for (int i = start_printing; i < total_bytes_read; i++) {
        if (first) {
            if (allResponse[i] == '}') {
                first = 0;
                start_printing = i + 3;
            }
        }
    }

    // Lastly print the email content
    print_slice(allResponse, start_printing, content);

    free(allResponse);



}