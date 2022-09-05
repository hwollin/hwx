# hwx

I/O多路复用在不同的系统上有不同的实现
+ Linux: epoll
+ BSD（包括MacOS）: kqueue
+ Windows: IOCP

目前暂不打算支持Windows

### 接口

+ kqueue()：创建kqueue对象
+ kevent()：注册事件并返回待办事件
```c
int 
kevent(int kq, //kqueue对象
       const struct kevent *changelist, //注册的所有事件
       int nchanges, //changelist的大小
       struct kevent *eventlist, //发生的事件会存储在这里，后续需要我们处理
       int nevents, //eventlist的大小
       const struct timespec *timeout); //超时时间，为NULL时会一直等待
```
+ EV_SET()：宏用于方便初始化kevent结构
+ kevent结构体
```c
 struct kevent {
     uintptr_t	ident;	     /*	identifier for this event  文件描述符 */
     short     filter;	     /*	filter for event  我们所关注的事件例如 EVFILT_READ... */
     u_short   flags;	     /*	action flags for kqueue	添加事件｜删除事件等等 */ 
     u_int     fflags;	     /*	filter flag value */
     int64_t   data;	     /*	filter data value */
     void      *udata;	     /*	opaque user data identifier */
     uint64_t  ext[4];	     /*	extensions */
 };
```
### 相关文档

+ [kqueue man page](https://www.freebsd.org/cgi/man.cgi?kqueue)
