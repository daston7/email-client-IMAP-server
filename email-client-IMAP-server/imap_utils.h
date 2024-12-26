#ifndef IMAP_UTILS_H
#define IMAP_UTILS_H

void login_to_imap(int sockfd, const char *username, const char *password);
void select_folder(int sockfd, const char *folder);

#endif
