# 压缩列表

用于存储小整形与短字符串的数据结构,目的是节约内存，属于双端链表的变种

## 数据结构

1. 列表数据结构  
    | 列表总字节数(4bytes) | 表头到表尾节点的偏移量（4bytes) | 节点数量（2bytes) | 节点a | 节点b ｜ 结束标志（0xFF) ｜
2. 节点数据结构  
    不是按照 `zlentry` 排列   
    | 前一个节点的字节数(1byte or 5bytes) | 当前节点的编码,记录类型与长度（1byte or 2 bytes or 5 bytes）| 节点值 ｜

每次插入或删除一个节点时要考虑修改节点的后一个节点的`prevlensize`是否能满足编码,可能需要将`prevlensize`由1个字节调整为5个字节  

`static unsigned char *__ziplistInsert(unsigned char *zl, unsigned char *p, unsigned char *s, unsigned int slen)`   
为插入节点的函数，理解该实现可以帮助理解压缩列表的结构  

压缩列表中的数据结构设计，内存移动、拷贝,位运算操作,宏函数等使用都十分经典,这里不展开介绍，建议直接阅读源码学习