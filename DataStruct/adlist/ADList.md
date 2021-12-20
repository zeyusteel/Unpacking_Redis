# A generic doubly linked list (双端链表)

双端链表的单个节点结构体与普通数双端链表结构体基本一致

```c
/*
 * 双端链表节点
 */
typedef struct listNode {

    // 前置节点
    struct listNode *prev;

    // 后置节点
    struct listNode *next;

    // 节点的值
    void *value;

} listNode;
```
对外不需要暴露`listNode`结构体，只需`list`结构体,这样获取表头、表尾、节点数量的时间复杂度都是O(1)
- 如果`free`函数不为空的话，在释放链表时需要调用`free`函数去释放`value`
- `dup`函数如果不为空的话，在拷贝链表时，会调用`dup`函数去复制`value`的值，否则拷贝后的链表将与原来的链表使用同一块内存的`value`
- `match`如果不为空将在函数匹配时`listNode *listSearchKey(list *list, void *key)`调用match函数去匹配key与value;如果`match`为空,匹配规则是`key`与`value`是否指向同一地址
```c
/*
 * 双端链表结构
 */
typedef struct list {

    // 表头节点
    listNode *head;

    // 表尾节点
    listNode *tail;

    // 节点值复制函数
    void *(*dup)(void *ptr);

    // 节点值释放函数
    void (*free)(void *ptr);

    // 节点值对比函数
    int (*match)(void *ptr, void *key);

    // 链表所包含的节点数量
    unsigned long len;

} list;
```