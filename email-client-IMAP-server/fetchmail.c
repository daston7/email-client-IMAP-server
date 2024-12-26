#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/socket.h>
#include "email_list.h"
#include "email_parser.h"
#include "imap_utils.h"
#include "mime_handler.h"
#include "net_utils.h"
#include "retrieve_handler.h"

// Protection against injection
int contains_newline(const char *str) {
    while (*str) {
        if (*str == '\r' && *(str + 1) == '\n') return 1;
        str++;
    }
    return 0;
}



int main(int argc, char *argv[]) {
    int opt, sockfd, use_tls = 0, port = 143;
    int messageNum = -1;
    char *username = NULL, *password = NULL, *folder = "INBOX", *server = NULL, *command = NULL;

    // Minimum required number of argument passed is 7
    if (argc < 7) {
        printf("Insuffcient arguments\n");
        exit(1);
    }

    // Parsing the command line arguments
    while ((opt = getopt(argc, argv, "u:p:f:n:t")) != -1) {
        switch (opt) {
            case 'u': {
                username = optarg;
                if (contains_newline(username)) {
                    exit(1);
                }
                break;
            }
            case 'p': {
                password = optarg;
                if (contains_newline(password)) {
                    exit(1);
                }
                break;
            }
            case 'f': folder = optarg; break;
            case 'n': {
                char *endptr;
                long val = strtol(optarg, &endptr, 10);
                if (*endptr != '\0' || optarg == endptr || val < 0 || val > INT_MAX) {
                    fprintf(stderr, "Invalid number: %s\n", optarg);
                    exit(1);
                }
                messageNum = val;
                break;
            }
            case 't': use_tls = 1; port = 993; break;
            default: {
                printf("Usage: fetchmail -u <username> -p <password> [-f <folder>] [-t] <command> <server_name>\n");
                exit(1);
            }
        }
    }

    // Command and server name in the command lines are optional
    if (optind + 2 != argc) {
        printf("Usage: fetchmail -u <username> -p <password> [-f <folder>] [-t] <command> <server_name>\n");
        exit(1);
        
    }

    // Assign command and server from command line arguments
    command = argv[optind];
    server = argv[optind + 1];

    // Establish socket connection to server
    sockfd = create_socket(server, port, use_tls);


    login_to_imap(sockfd, username, password);
    select_folder(sockfd, folder);

    // Determine logic based on input of CLI
    if (strcmp(command, "retrieve") == 0) {
        retrieve_email(sockfd, messageNum);
    } else if (strcmp(command, "parse") == 0) {
        parse_email(sockfd, messageNum);
    } else if (strcmp(command, "mime") == 0) {
        mime_email(sockfd, messageNum);
    } else if (strcmp(command, "list") == 0) {
        list_emails(sockfd);
    } else {
        printf("Unsupported command provided\n");
        exit(1);
    }
    // Socket close 
    close(sockfd);
    return 0;
}