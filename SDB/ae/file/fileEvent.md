## 文件事件
redis客户端与服务端通过套接字连接,文件事件就是服务器对redis套接字的抽象
## 文件事件处理器
使用IO多路复用来监听多个套接字，并对套接字执行的任务关联不同的事件处理器

![文件事件处理器](resource/event.png)
虽然文件事件会并发出现，但IO多路复用程序会将套接字事件放到同步队列里，按顺序发送给文件事件分配器

## IO多路复用程序
对select,epoll,kqueue,等多路复用函数的封装，类似于策略模式,会在编译阶段选择最合适的函数库
```c
#ifdef HAVE_EVPORT
#include "ae_evport.c"
#else
    #ifdef HAVE_EPOLL
    #include "ae_epoll.c"
    #else
        #ifdef HAVE_KQUEUE
        #include "ae_kqueue.c"
        #else
        #include "ae_select.c"
        #endif
    #endif
#endif
```
## 文件事件结构
```c
/* File event structure
 *
 * 文件事件结构
 */
typedef struct aeFileEvent {

    // 监听事件类型掩码，
    // 值可以是 AE_READABLE 或 AE_WRITABLE ，
    // 或者 AE_READABLE | AE_WRITABLE
    int mask; /* one of AE_(READABLE|WRITABLE) */

    // 读事件处理器
    aeFileProc *rfileProc;

    // 写事件处理器
    aeFileProc *wfileProc;

    // 多路复用库的私有数据
    void *clientData;

} aeFileEvent;
```

`aeCreateFileEvent` //根据 mask 参数的值，监听 fd 文件的状态

`aeDeleteFileEvent` //将 fd 从 mask 指定的监听队列中删除

`aeGetFileEvents` //获取给定 fd 正在监听的事件类型

`aeWait` //在给定毫秒内等待，直到 fd 变成可写、可读或异常 (poll)

`aeApiPoll` //在指定时间内阻塞等待所有 `aeCreateFileEvent` 设置监听的套接字,当至少一个事件或超时，函数返回,更新就绪的文件事件 aeFiredEvent *fired; /* Fired events */

## 文件事件分配器
`aeProcessEvents`  //处理所有已到达的时间事件，以及所有已就绪的文件事件。

