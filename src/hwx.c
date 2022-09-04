//
// Created by hw on 2022/9/2.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

#define CONN_QUEUE_SIZE 10
#define BUFF_SIZE 1024
#define INDEX_HTML "../html/index.html"

//在http1.1及之后版本。如果是keep alive，则content-length和chunk必然是二选一
//否则当你用浏览器访问时，响应体会无法显示
#define RESP_OK "HTTP/1.1 200 OK\r\nServer: hwx\r\nContent-Length: 144\r\nContent-Type: text/html; charset=UTF-8\r\n\r\n"

void write_response_to_buff(const char *file, char *buff);

int main(int argc, char *const *argv) {
    // <1> server socket
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);


    // <2> server socket address info
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));

    // IPv4 Internet protocols
    servaddr.sin_family = AF_INET;

    // converts the unsigned short integer 80 from host byte order to network byte order.
    servaddr.sin_port = htons(9999);

    // Address to accept any incoming messages.
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);


    // <2> -> <1> : assign address "servaddr" to socket "listen_sock"
    bind(listen_sock, (struct sockaddr *) &servaddr, sizeof(servaddr));

    // <3> listen for connections on "listen_sock", 10 connection requests will be queued before
    // further requests are refused.
    listen(listen_sock, CONN_QUEUE_SIZE);

    // <4.1> client address info
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;

    // <6.1> buffer to read request
    char req_buff[BUFF_SIZE] = {'\0'};
    // 单次recv()接收到的字节数
    int recv_len = 0;

    // <7.1> buffer to send response
    char resp_buff[BUFF_SIZE] = {'\0'};
    write_response_to_buff(INDEX_HTML, resp_buff);

    while (1) {
        // <4> Await a connection on socket "listen_sock". When a connection arrives, open a new socket
        // to communicate with it, and return the new socket's fd
        int conn_sock = accept(listen_sock, (struct sockaddr *) &cliaddr, &cliaddr_len);

        // <5> print client address info
        // inet_ntoa : converts the Internet host address "cliaddr.sin_addr" to "string ip address"
        printf("connected from %s\n", inet_ntoa(cliaddr.sin_addr));

        // <6> read request
        // recv() returns the number of bytes received, if no message are avaiable at the
        // socket, the recv() func wait for a message to arrive.
        // 如果请求的大小超过了buff的大小，那就多次读取
        do {
            recv_len = recv(conn_sock, req_buff, BUFF_SIZE, 0);
            printf("%s", req_buff);
            memset(req_buff, 0, sizeof(req_buff));
        } while (recv_len == BUFF_SIZE);
        printf("\n");

        // <7> send index.html to client
        // 仅仅是把resp_buff中的数据copy到发送缓冲区，而不会真正等待发送完成
        send(conn_sock, resp_buff, strlen(resp_buff), 0);

        // <8> close conn socket
        //close(conn_sock);
    }
}

// 自己构造一个http响应，将index.html返回给客户端
void write_response_to_buff(const char *file, char *buff) {
    int header_len = strlen(RESP_OK);

    // 响应头
    memcpy(buff, RESP_OK, header_len);

    // 响应体
    FILE *f = fopen(file, "rb");
    fread(buff + header_len, 1, BUFF_SIZE, f);
    fclose(f);
}