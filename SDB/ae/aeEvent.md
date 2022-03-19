# 事件调度

redis服务端的核心就是事件处理,服务起来之后就是一直运行事件处理器，一直到服务器关闭为止
```c
/*
 * 事件处理器的主循环
 */
void aeMain(aeEventLoop *eventLoop) {

    eventLoop->stop = 0;

    while (!eventLoop->stop) {

        // 如果有需要在事件处理前执行的函数，那么运行它
        if (eventLoop->beforesleep != NULL)
            eventLoop->beforesleep(eventLoop);

        // 开始处理事件
        aeProcessEvents(eventLoop, AE_ALL_EVENTS);
    }
}

```

aeProcessEvents流程：
1. 获取最近的时间事件
2. 计算最近的时间时间距离当前时间还有多少毫秒
3. 依据这个时间对文件事件进行阻塞监听
4. 处理文件事件
5. 处理时间时间
