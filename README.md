# hwx
在zero分支的基础上，添加epoll

### epoll简介

+ monitoring multiple file descriptors to see if I/O is possible on any of them.
+ The epoll API can be used either as an edge-triggered or a level-triggered
    + level-t(default): 只要监听的文件描述符中有数据，就会触发epoll_wait返回
    + edge-t:  只有监听的文件描述符的读/写事件发生，才会触发epoll_wait返回
+ epoll instance is an in-kernel data structure which has two lists:
    + interest list: all fds
    + ready list: fds that are ready for I/O

### 相关文档
#### epoll
+ [epoll man page](https://man7.org/linux/man-pages/man7/epoll.7.html)
    + epoll_create(): create epoll instance
    + epoll_ctl(): add/del/... fd to epoll
    + epoll_wait(): wait for I/O events, blocking if no events are available
+ [水平触发和边沿触发](https://zhuanlan.zhihu.com/p/107995399)
+ epoll events:
  + [EPOLLIN和EPOLLOUT的触发条件](https://blog.csdn.net/zgaoq/article/details/103807337)
