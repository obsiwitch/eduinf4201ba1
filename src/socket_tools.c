#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include "socket_tools.h"

#define BUFFER_LEN 1024

/**
* Initialize a stream socket used as a server.
* Exits the application if one of the steps to create and initialize the socket
* fails.
*
* @param port on which the socket will be bound
* @return File descriptor of the created socket.
*/
int init_stream_server_socket(int port) {
    int sockfd, status;
    struct sockaddr_in serv_addr;

    // Create socket (IPv4, connected, TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("server - socket init");
        exit(EXIT_FAILURE);
    }

    // Fill serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    memset(serv_addr.sin_zero, 0, sizeof(serv_addr.sin_zero));

    // Bind socket to all local interfaces on the port specified in argument
    status = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (status == -1) {
        perror("server - bind");
        exit(EXIT_FAILURE);
    }

    // Passive (listening) socket
    status = listen(sockfd, 1);
    if (status == -1) {
        perror("server - listen");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

/**
 * Send a complete message with the connectionless socket specified in parameter
 * (sockfd file descriptor).
 * @param sockfd sending connectionless socket
 * @param msg message to send
 * @param msg_size size of the message to send
 * @param dest_addr address of the target
 * @return Returns 0 on success, and -1 if the message could not completely be
 * sent.
 */
int sendto_complete(int sockfd, char* msg, int msg_size,
    const struct sockaddr *dest_addr)
{
    int sent_size = 0;

    while (sent_size < msg_size) {
        int remaining_size = msg_size - sent_size;
        int temp_size = sendto(sockfd, msg + sent_size, remaining_size, 0,
        dest_addr, sizeof(*dest_addr));
        if (temp_size == -1) {
            perror("sendto");
            break;
        }

        sent_size += temp_size;
    }

    return (sent_size != msg_size) ? -1 : 0;
}

/**
 * Send a complete message with the connected socket specified in parameter
 * (sockfd file descriptor).
 *
 * @param sockfd sending connected socket
 * @param msg message to send
 * @param msg_size size of the message to send
 * @return Returns 0 on success, and -1 if the message could not completely be
 * sent.
 */
int send_complete(int sockfd, char* msg, int msg_size) {
    int sent_size = 0;

    while (sent_size < msg_size) {
        int remaining_size = msg_size - sent_size;
        int temp_size = send(sockfd, msg + sent_size, remaining_size, 0);
        if (temp_size == -1) {
            perror("send");
            break;
        }

        sent_size += temp_size;
    }

    return (sent_size != msg_size) ? -1 : 0;
}

/**
 * Receive a message from a socket and adds a trailing '\0' to it.
 * @return Returns 0 on success, -1 otherwise.
 */
int recvfrom_helper(int sockfd, char *buffer, int buffer_size, int *recv_size,
    struct sockaddr *src_addr, socklen_t *addrlen)
{
    *recv_size = recvfrom(sockfd, buffer, buffer_size, 0, src_addr, addrlen);
    if (*recv_size == -1) {
        perror("recvfrom");
        return -1;
    }

    // Make sure the received message ends with '\0'.
    if (buffer[*recv_size - 1] != '\0') {
        if (*recv_size == buffer_size) {
            buffer[*recv_size - 1] = '\0';
        }
        else {
            buffer[*recv_size] = '\0';
        }
    }

    return 0;
}

/**
 * Receive a complete message by making multiple recv calls, and each time
 * a recv call returns, prints it.
 *
 * @param sockfd
 * @return 0 on success, -1 otherwise.
*/
int recv_print(int sockfd) {
    int recv_size;
    char buf[BUFFER_LEN];

    do {
        recv_size = read(sockfd, buf, BUFFER_LEN - 1);
        if (recv_size == -1) {
            perror("read");
            break;
        }

        if (recv_size != 0) {
            buf[recv_size] = '\0';
            puts(buf);
        }

        if (strstr(buf, "\r\n\r\n") != NULL) {
            break;
        }
    } while(recv_size != 0);

    return recv_size;
}
