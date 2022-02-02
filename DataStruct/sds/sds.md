# Simple Dynamic String (简单动态字符串)
## 01 源代码：src/sds.c 、src/sds.h
## 02 数据结构：
### 3.0结构
```c
/*
 * 保存字符串对象的结构
 */
struct sdshdr {
    
    // buf 中已占用空间的长度
    int len;

    // buf 中剩余可用空间的长度
    int free;

    // 数据空间
    char buf[];     //数组指针，不占用存储空间
};
```
特点:
* len保存字符串长度让查询字符串长度时间复杂度从O(N)降到O(1)
* len保存字符串长度符串长度避免字符拼接等操作出现缓冲区溢出
* free保存可用空间减少内存重分配时系统调用的次数
    * 空间预分配
        * 修改sds如果len小于1M,会分配和len等长的free空间
        * 修改sds如果len大于等于1M,会分配1M的free空间
    * 惰性空间释放
        * sds字符串缩短后并不会立即回收内存，而是记录到free，方便下次使用

* sds是二进制安全的，解决中间出现`\0`的问题
* sds兼容<string.h>字符串操作  

引用《Redis设计与实现》与C字符串对比
![与C字符串对比](resource/compare.png)
引用《Redis设计与实现》api
![与C字符串对比](resource/api.png)
***

### 6.2.6
```c
/* Note: sdshdr5 is never used, we just access the flags byte directly.
 * However is here to document the layout of type 5 SDS strings. */
struct __attribute__ ((__packed__)) sdshdr5 {
    unsigned char flags; /* 3 lsb of type, and 5 msb of string length */
    char buf[];
};
struct __attribute__ ((__packed__)) sdshdr8 {
    uint8_t len; /* used */
    uint8_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__ ((__packed__)) sdshdr16 {
    uint16_t len; /* used */
    uint16_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__ ((__packed__)) sdshdr32 {
    uint32_t len; /* used */
    uint32_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
struct __attribute__ ((__packed__)) sdshdr64 {
    uint64_t len; /* used */
    uint64_t alloc; /* excluding the header and null terminator */
    unsigned char flags; /* 3 lsb of type, 5 unused bits */
    char buf[];
};
```
区别：  
- `__attribute__ ((__packed__))` 取消字节对齐，方便直接通过内存访问结构体内部变量  
- len、alloc、flags组成sds的header  
- len与3.0版本一致，alloc保存除了头部和buf最后默认携带的`\0`的长度。

## 03随记
1. 
```c
#define update_zmalloc_stat_alloc(__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    if (zmalloc_thread_safe) { \
        update_zmalloc_stat_add(_n); \
    } else { \
        used_memory += _n; \
    } \
} while(0)
```

因为 sizeof(long) = 8 (64位系统中)，所以上面的第一个if语句，可以等价于以下代码:  
     if(_n&7) _n += 8 - (_n&7);

这段代码就是判断分配的内存空间的大小是不是8的倍数。如果内存大小不是8的倍数，就加上相应的偏移量使之变成8的倍数。_n&7 在功能上等价于 _n%8，不过位操作的效率显然更高

2. `do {} while(0)` 用处
- 辅助定义复杂的宏，避免引用的时候出错 
- 避免使用goto对程序流进行统一的控制,用break 
- 避免空宏引起的warning 
- 定义一个单独的函数块来实现复杂的操作

3. __attribute__((format(printf, 2, 3)));   
编译时检查格式错误
__attribute__((format(printf, m, n)))  
__attribute__((format(scanf, m, n)))  

其中参数m与n的含义为：
- m：第几个参数为格式化字符串(format string);  
- n：参数集合中的第一个，即参数“…”里的第一个参数在函数参数总数排在第几  

4. memmove可以对覆盖的字符拷贝
```c
    // 如果有需要，前移字符串内容
    // T = O(N)
    if (sh->buf != sp) memmove(sh->buf, sp, len);
```
