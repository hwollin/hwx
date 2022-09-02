//
// Created by hw on 2022/9/2.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define CONN_QUEUE_SIZE 10

int main(int argc, char *const *argv)
{
    // <1> server socket
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);


    // <2> server socket address info
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    // IPv4 Internet protocols
    servaddr.sin_family = AF_INET;

    // converts the unsigned short integer 80 from host byte order to network byte order.
    servaddr.sin_port = htons(80);

    // Address to accept any incoming messages.
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);


    // <2> -> <1> : assign address "servaddr" to socket "listen_sock"
    bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // <3> listen for connections on "listen_sock", 10 connection requests will be queued before
    // further requests are refused.
    listen(listen_sock, CONN_QUEUE_SIZE);

    // <4.1> client address info
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;

    while (1)
    {
        // <4> Await a connection on socket "listen_sock". When a connection arrives, open a new socket
        // to communicate with it, and return the new socket's fd
        int conn_sock = accept(listen_sock, (struct sockaddr*)&cliaddr, &cliaddr_len);

        // <5> print client address info
        // inet_ntoa : converts the Internet host address "cliaddr.sin_addr" to "string ip address"
        printf("connected from %s\n", inet_ntoa(cliaddr.sin_addr));

        // <6> close conn socket
        close(conn_sock);
    }

}