//
// Created by hw on 2022/9/2.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

#define CONN_QUEUE_SIZE 1024
#define BUFF_SIZE 1024
#define INDEX_HTML "../html/index.html"
#define MAX_EVENTS 1024

//在http1.1及之后版本。如果是keep alive，则content-length和chunk必然是二选一
//否则当你用浏览器访问时，响应体会无法显示
#define RESP_OK "HTTP/1.1 200 OK\r\nServer: hwx\r\nContent-Length: 144\r\nContent-Type: text/html\r\nConnection: keep-alive\r\n\r\n"

void write_response_to_buff(const char *file, char *buff);

void setnonblocking(int sock);

int main(int argc, char *const *argv) {
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);

    setnonblocking(listen_sock);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8088);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listen_sock, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listen_sock, CONN_QUEUE_SIZE);

    // client address info
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;

    // buffer to read request
    char req_buff[BUFF_SIZE] = {'\0'};
    // 单次recv()接收到的字节数
    int recv_len = 0;

    // buffer to send response
    char resp_buff[BUFF_SIZE] = {'\0'};
    write_response_to_buff(INDEX_HTML, resp_buff);

    // epoll
    int epollfd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;

    // number of fd that events happen
    int nfds;

    // add listen_sock to epoll
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev);

    int flag;
    while (1) {
        //阻塞直到有事件发生
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);

        //遍历发生的事件
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listen_sock) {  // 有新的连接到来
                int conn_sock = accept(listen_sock, (struct sockaddr *) &cliaddr, &cliaddr_len);
                setnonblocking(conn_sock);

                // print client ip
                printf("hello %s\n", inet_ntoa(cliaddr.sin_addr));

                // add conn_sock to epoll
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev);
            } else if (events[i].events & EPOLLIN) {  // conn_sock read
                int conn_sock = events[i].data.fd;

                // print request
                do {
                    recv_len = recv(conn_sock, req_buff, BUFF_SIZE, 0);

                    // client close connection
                    if(recv_len == 0) {
                        //TODO 将该连接删除

                        flag = -1;
                    }

                    printf("%s", req_buff);
                    memset(req_buff, 0, sizeof(req_buff));
                } while (recv_len == BUFF_SIZE);

                if(flag == -1) {
                    flag = 0;
                    continue;
                }
                printf("\n");

                // 获取请求之后要去响应
                ev.events = EPOLLOUT | EPOLLET;
                ev.data.fd = conn_sock;
                // 会触发EPOLLOUT事件
                epoll_ctl(epollfd, EPOLL_CTL_MOD, conn_sock, &ev);
            } else if (events[i].events & EPOLLOUT) {  // conn_sock write
                int conn_sock = events[i].data.fd;
                // send response
                send(conn_sock, resp_buff, strlen(resp_buff), 0);

                // 响应后要继续关注请求
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                epoll_ctl(epollfd, EPOLL_CTL_MOD, conn_sock, &ev);
            }

        }
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

// 将socket(默认阻塞)设置为非阻塞
void setnonblocking(int sock) {
    int opts = fcntl(sock, F_GETFL);
    fcntl(sock, F_SETFL, opts | O_NONBLOCK);
}