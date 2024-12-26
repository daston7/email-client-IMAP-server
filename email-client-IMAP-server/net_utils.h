#ifndef NET_UTILS_H
#define NET_UTILS_H

#define BUFFER_SIZE 2048

void send_command(int sockfd, const char *command);
int read_response(int sockfd, char *buffer);
int create_socket(const char *hostname, int port, int use_tls);
void print_slice(const char *response, int start, int length);

#endif