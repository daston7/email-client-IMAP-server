#include "net_utils.h"
#include "email_list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Parse and print email subjects from server response
void parse_and_print_subjects(const char *response) {
    int messageNum = 1;
    const char *ptr = response;

    // Loop through the response
    while (*ptr) {
        const char *fetch_start = strstr(ptr, "*");
        if (!fetch_start) break;

        const char *fetch_end = strstr(fetch_start, ")\r\n");
        if (!fetch_end) break;
        fetch_end += 3;

        // Search for subject within message boundaries
        const char *subject_start = strstr(fetch_start, "Subject:");
        if (subject_start && subject_start < fetch_end) {
            // Skip past subject
            subject_start += 8;
            while (*subject_start == ' ' || *subject_start == '\r' || *subject_start == '\n') subject_start++;

            const char *subject_end = strstr(subject_start, "\r\n");
            
            while (subject_end && subject_end < fetch_end) {
                const char *next_line_start = subject_end + 2;
                if (*next_line_start == ' ' || *next_line_start == '\t') {
                    subject_end = strstr(next_line_start, "\r\n");
                } else {
                    break; 
                }
            }

            // Print subject line
            if (!subject_end || subject_end > fetch_end) subject_end = fetch_end - 3;

            printf("%d: ", messageNum);
            int is_first_print_char = 1;
            while (subject_start < subject_end) {
                if (*subject_start != '\r' && *subject_start != '\n') {
                    if (is_first_print_char && *subject_start == ' ') {
                        subject_start++; 
                        continue;
                    }
                    putchar(*subject_start);
                    is_first_print_char = 0; 
                }
                subject_start++;
            }
            printf("\n");
        } else {
            // If email doesn't have a subject
            printf("%d: <No subject>\n", messageNum);
        }

        ptr = fetch_end; 
        messageNum++;
    }
}




// Function to list emails
void list_emails(int sockfd) {
    char command[BUFFER_SIZE];

    // Dynamically allocate memory storing server responses
    int allResponseCap = BUFFER_SIZE * 10;
    char* allResponse = (char*)malloc(allResponseCap * sizeof(char));
    if (!allResponse) {
        perror("Failed to allocate memory");
        exit(1);
    }
    allResponse[0] = '\0';


    int total_bytes_read = 0;
    int bytes_read;


    // Send command to server to fetch all subject
    snprintf(command, sizeof(command), "A06 FETCH 1:* (BODY.PEEK[HEADER.FIELDS (SUBJECT)])\r\n");
    send_command(sockfd, command);

    char response[BUFFER_SIZE];
    // Read server's response
    while((bytes_read = read_response(sockfd, response)) > 0) {
        // Check if buffer is available and expand its size when needed
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
        // Append responses
        strcat(allResponse, response);


        total_bytes_read += bytes_read;


        // When server indicates completion
        if (strstr(allResponse, "A06 OK")) {
            break;
        }

    }
    allResponse[total_bytes_read] = '\0';



    if (allResponse) {
        parse_and_print_subjects(allResponse);
    }

    free(allResponse);
}