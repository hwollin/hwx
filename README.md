# hwx
实现最简单的http服务器的功能，其他分支将在该分支基础上不断添加新功能

目前功能：
+ 打印客户端的ip地址，打印请求
+ 响应index.html页面给客户端

### 遇到的问题
响应头中必须加上Content-Length（响应体长度，response-body）字段，否则当用浏览器访问时会无法获取到
响应体

### 相关接口文档：
#### 从socket发送和接收数据
+ [send函数](https://man7.org/linux/man-pages/man2/send.2.html)
+ [recv函数](https://man7.org/linux/man-pages/man2/recv.2.html)

#### 读取文件到buffer
+ [fread函数](https://man7.org/linux/man-pages/man3/fread.3.html)