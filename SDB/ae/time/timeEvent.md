# 事件事件

所有时间事件存在一个链表里面，每次处理时间都会遍历这个链表，处理到达的时间事件
```c
/* Time event structure
 *
 * 时间事件结构
 */
typedef struct aeTimeEvent {

    // 时间事件的唯一标识符
    long long id; /* time event identifier. */

    // 事件的到达时间
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */

    // 事件处理函数
    aeTimeProc *timeProc;

    // 事件释放函数
    aeEventFinalizerProc *finalizerProc;

    // 多路复用库的私有数据
    void *clientData;

    // 指向下个时间事件结构，形成链表
    struct aeTimeEvent *next;

} aeTimeEvent;
```

## api

- `aeCreateTimeEvent` //创建时间事件
- `aeDeleteTimeEvent` //删除给定 id 的时间事件
- `processTimeEvents` //处理事件事件
    遍历链表  
    如果时间到达，执行时间事件  
    如果不是循环事件，从链表删除，否则更新下次到达时间


## redis 中主要的时间事件 serverCron
```c

/* This is our timer interrupt, called server.hz times per second.
 *
 * 这是 Redis 的时间中断器，每秒调用 server.hz 次。
 *
 * Here is where we do a number of things that need to be done asynchronously.
 * For instance:
 *
 * 以下是需要异步执行的操作：
 *
 * - Active expired keys collection (it is also performed in a lazy way on
 *   lookup).
 *   主动清除过期键。
 *
 * - Software watchdog.
 *   更新软件 watchdog 的信息。
 *
 * - Update some statistic.
 *   更新统计信息。
 *
 * - Incremental rehashing of the DBs hash tables.
 *   对数据库进行渐增式 Rehash
 *
 * - Triggering BGSAVE / AOF rewrite, and handling of terminated children.
 *   触发 BGSAVE 或者 AOF 重写，并处理之后由 BGSAVE 和 AOF 重写引发的子进程停止。
 *
 * - Clients timeout of different kinds.
 *   处理客户端超时。
 *
 * - Replication reconnection.
 *   复制重连
 *
 * - Many more...
 *   等等。。。
 *
 * Everything directly called here will be called server.hz times per second,
 * so in order to throttle execution of things we want to do less frequently
 * a macro is used: run_with_period(milliseconds) { .... }
 *
 * 因为 serverCron 函数中的所有代码都会每秒调用 server.hz 次，
 * 为了对部分代码的调用次数进行限制，
 * 使用了一个宏 run_with_period(milliseconds) { ... } ，
 * 这个宏可以将被包含代码的执行次数降低为每 milliseconds 执行一次。
 */

    /* Create the serverCron() time event, that's our main way to process
     * background operations. */
    // 为 serverCron() 创建时间事件
    if(aeCreateTimeEvent(server.el, 1, serverCron, NULL, NULL) == AE_ERR) {
        redisPanic("Can't create the serverCron time event.");
        exit(1);
    }

```