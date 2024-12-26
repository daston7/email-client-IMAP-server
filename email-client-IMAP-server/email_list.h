#ifndef EMAIL_LIST_H
#define EMAIL_LIST_H

void list_emails(int sockfd);
void parse_and_print_subjects(const char *response);

#endif