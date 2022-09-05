# hwx
实现最简单的http服务器的功能，其他分支将在该分支基础上不断添加新功能

目前功能：
+ 打印客户端的ip地址，打印请求
+ 响应index.html页面给客户端

### 相关接口文档：
#### http响应头格式
+ [HTTP响应格式](https://blog.csdn.net/Emperor10/article/details/123192456)
+ 随便访问个网站然后wireshark抓包

#### 从socket发送和接收数据
+ [send函数](https://man7.org/linux/man-pages/man2/send.2.html)
+ [recv函数](https://man7.org/linux/man-pages/man2/recv.2.html)

#### 读取文件到buffer
+ [fread函数](https://man7.org/linux/man-pages/man3/fread.3.html)


### 遇到的问题
#### 已解决
+ 响应头中必须加上Content-Length（响应体长度，response-body）字段，否则当用浏览器访问时会无法获取到响应体
+ 无法实现连接的复用，从chrome多次请求127.0.0.1，会发现每次都会创建一个新的连接，epoll可以解决该问题
```c
int conn_sock = accept(listen_sock, (struct sockaddr *) &cliaddr, &cliaddr_len);
printf("connection----------->%d\n", conn_sock);
```

#### 待解决
+ 如何优雅的关闭socket？这么关闭是有问题的
```c
// <8> close conn socket
close(conn_sock);
```
