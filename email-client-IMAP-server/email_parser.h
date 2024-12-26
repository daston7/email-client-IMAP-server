#ifndef EMAIL_PARSER_H
#define EMAIL_PARSER_H

void parse_email(int sockfd, int messageNum);
char *normalize_header(const char *header);
void capitalize_header_name(char *header);

#endif