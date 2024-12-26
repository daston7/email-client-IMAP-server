#include "email_parser.h"
#include "net_utils.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

// Function that converts email header keys to lower-cased
char *normalize_header(const char *header) {
    static char norm_header[1024];
    int i = 0;
    while (header[i] && header[i] != ':') {
        norm_header[i] = tolower(header[i]);
        i++;
    }
    strcpy(norm_header + i, header + i);
    return norm_header;
}


// Function to captilize header names
void capitalize_header_name(char *header) {
    char *colon_pos = strchr(header, ':');
    if (colon_pos) {
        int len = colon_pos - header;
        if (strncasecmp(header, "from", len) == 0) {
            header[0] = 'F';
            header[1] = 'r';
            header[2] = 'o';
            header[3] = 'm';
        }
        else if (strncasecmp(header, "to", len) == 0) {
            header[0] = 'T';
            header[1] = 'o';
        }
        else if (strncasecmp(header, "date", len) == 0) {
            header[0] = 'D';
            header[1] = 'a';
            header[2] = 't';
            header[3] = 'e';
        }
        else if (strncasecmp(header, "subject", len) == 0) {
            header[0] = 'S';
            header[1] = 'u';
            header[2] = 'b';
            header[3] = 'j';
            header[4] = 'e';
            header[5] = 'c';
            header[6] = 't';
        }
    }

}
// Function that parses email
void parse_email(int sockfd, int messageNum) {
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
        snprintf(command, sizeof(command), "A04 FETCH * (BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)])\r\n");
    } else {
        snprintf(command, sizeof(command), "A04 FETCH %d (BODY.PEEK[HEADER.FIELDS (FROM TO DATE SUBJECT)])\r\n", messageNum);
    }
    
    send_command(sockfd, command);

    // Read response from server and store them
    while((bytes_read = read_response(sockfd, response)) > 0) {

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

        strcat(allResponse, response);


        total_bytes_read += bytes_read;

        // Check if server has finished sending response
        if (strstr(allResponse, "A04 OK")) {
            break;
        }

    }
    allResponse[total_bytes_read] = '\0';






    // Process response
    if (allResponse) {
        char *line = strtok(allResponse, "\r\n");
        char current_header[1024] = {0};
        char *headers[4] = {NULL, NULL, NULL, NULL};

        while (line) {
            // Skip lines that don't contain header information
            if (line[0] == '*' || strncmp(line, "A04 OK", 6) == 0 || line[0] == ')') {
                line = strtok(NULL, "\r\n");
                continue; 
            }

            if (line[0] == ' ' || line[0] == '\t') { 
                strcat(current_header, line);
            } else {
                if (current_header[0] != '\0') {
                    capitalize_header_name(current_header);
                    if (strstr(current_header, "From:")) headers[0] = strdup(current_header);
                    else if (strstr(current_header, "To:")) headers[1] = strdup(current_header);
                    else if (strstr(current_header, "Date:")) headers[2] = strdup(current_header);
                    else if (strstr(current_header, "Subject:")) headers[3] = strdup(current_header);
                    current_header[0] = '\0'; 
                }
                strcpy(current_header, line);
            }
            line = strtok(NULL, "\r\n");
        }

        // Process last header
        if (current_header[0] != '\0') {
            capitalize_header_name(current_header);
            if (strstr(current_header, "From:")) headers[0] = strdup(current_header);
            else if (strstr(current_header, "To:")) headers[1] = strdup(current_header);
            else if (strstr(current_header, "Date:")) headers[2] = strdup(current_header);
            else if (strstr(current_header, "Subject:")) headers[3] = strdup(current_header);
        }

    
        if (!headers[0]) headers[0] = strdup("From: <Unknown sender>");
        if (!headers[1]) headers[1] = strdup("To:");
        if (!headers[2]) headers[2] = strdup("Date: <Unknown date>");
        if (!headers[3]) headers[3] = strdup("Subject: <No subject>");


        for (int i = 0; i < 4; i++) {
            if (headers[i]) {
                printf("%s\n", headers[i]);
                free(headers[i]);  
            }
        }
    } else {
        printf("Failed to fetch headers\n");
        free(allResponse);
        exit(1);
    }

    free(allResponse);
}
