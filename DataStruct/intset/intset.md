# 整数集合
特点：都是整形，由小到大排序，无重复数据，每个数据字段长度类型一致
```c
/*
 * intset 的编码方式
 */
#define INTSET_ENC_INT16 (sizeof(int16_t))
#define INTSET_ENC_INT32 (sizeof(int32_t))
#define INTSET_ENC_INT64 (sizeof(int64_t))
```
整数集合依据`encoding`字段编码方式，调整元素数组存储的数据类型  
当新插入的数据比当前编码类型都要大或者都要小时，对集合进行升级，按新的编码类型扩充contents长度，重新调整元素位置，最后插入新元素 
无法降级

```c
typedef struct intset {
    
    // 编码方式
    uint32_t encoding;

    // 集合包含的元素数量
    uint32_t length;

    // 保存元素的数组
    int8_t contents[];

} intset;
```

整数集合数据由小到大排序，查找采用的是二分查找
```c
    // 在有序数组中进行二分查找
    // T = O(log N)
    while(max >= min) {
        mid = (min+max)/2;
        cur = _intsetGet(is,mid);
        if (value > cur) {
            min = mid+1;
        } else if (value < cur) {
            max = mid-1;
        } else {
            break;
        }
    }
```

## 随记
- `memrev16`,`intrev16`,调整大小端存储方式，默认是小端存储
- `memmove`进行内存移动
- `rand`,`random` 获取随机值 `rand`返回值是int, `random`返回值long, 一般使用`srand` `srandom`设置随机种子(置入getpid(),time(0)),否则默认种子就是1,每次产生的随机数都一样