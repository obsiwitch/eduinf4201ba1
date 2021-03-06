#ifndef SOCKET_TOOLS_H
#define SOCKET_TOOLS_H

#include <netinet/in.h>

int init_stream_server_socket(int port);
int init_stream_client_socket(const char* hostname, int port);
int init_stream_client_socket_alt(const char* hostname, const char *port);
int sendto_complete(int sockfd, char* msg, int msg_size,
    const struct sockaddr *dest_addr);
int send_complete(int sockfd, char* msg, int msg_size);
int recvfrom_helper(int sockfd, char *buffer, int buffer_size, int *recv_size,
    struct sockaddr *src_addr, socklen_t *addrlen);
int recv_print(int sockfd);

#endif // SOCKET_TOOLS_H
