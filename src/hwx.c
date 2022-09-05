//
// Created by hw on 2022/9/2.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

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

    // kqueue
    int kq = kqueue();
    // 代表单个事件
    struct kevent ev;
    // 事件列表
    struct kevent evList[MAX_EVENTS];

    // 宏用于初始化kevent结构体
    EV_SET(&ev, listen_sock, EVFILT_READ, EV_ADD, 0, 0, 0);

    // The kevent() system call is used to register events with the queue, and
    //     return any	pending	events to the user
    kevent(kq, &ev, 1, NULL, 0, NULL);

    //用于while循环
    int event_num, i, flag;
    while (1) {
        //阻塞直到有事件发生
        event_num = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);

        //遍历发生的事件
        for (i = 0; i < event_num; i++) {
            if (evList[i].ident == listen_sock) {  // 有新的连接到来
                int conn_sock = accept(listen_sock, (struct sockaddr *) &cliaddr, &cliaddr_len);
                setnonblocking(conn_sock);

                // print client ip
                printf("hello %s\n", inet_ntoa(cliaddr.sin_addr));

                // add conn_sock to kqueue
                EV_SET(&ev, conn_sock, EVFILT_READ, EV_ADD, 0, 0, 0);
                kevent(kq, &ev, 1, NULL, 0, NULL);
            } else if (evList[i].filter & EVFILT_READ) {  // conn_sock read
                int conn_sock = evList[i].ident;

                // print request
                do {
                    recv_len = recv(conn_sock, req_buff, BUFF_SIZE, 0);
                    //返回-1表示出错，但真正的错误原因请看errno
                    if(recv_len == -1) {
                        //客户端关闭连接时删除事件
                        if(errno == ECONNRESET) {
                            EV_SET(&ev, conn_sock, EVFILT_READ, EV_DELETE, 0, 0, 0);
                            kevent(kq, &ev, 1, NULL, 0, NULL);
                        }
                        flag = -1;
                    }
                    printf("%s", req_buff);
                    memset(req_buff, 0, sizeof(req_buff));
                } while (recv_len == BUFF_SIZE);

                //发生错误时跳出本次循环处理下个事件
                if(flag == -1) {
                    flag = 0;
                    continue;
                }
                printf("\n");

                send(conn_sock, resp_buff, strlen(resp_buff), 0);
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