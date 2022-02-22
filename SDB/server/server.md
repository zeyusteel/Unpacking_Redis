# redis服务端

## main
1. `spt_init(argc, argv);` // 用于修改应用名称  
   原理:argv[0]是应用的名称，但是argv与environ都是指针数组，指向在一片连续的内存，如果修改后的argv[0]大于原来的进程名，会导致后面的缓冲区被覆盖, 这时我们保留argv[0]指向的那片内存，只用作新进程名修改，argv[1]..... environ[0]...都指向从新分配的内存（argv采用`strdup`,environ采用`setenv`,这两个函数都会重新分配内存

    这样我们只需要修改`base`就能修改进程名，而且不影响其他argv与后面的环境变量

```c
     static struct {
	    /* original value */
    	const char *arg0;

	    /* title space available */
	    char *base, *end;

	    /* pointer to original nul character within base */
	    char *nul;

	    _Bool reset;
	    int error;
    } SPT;
```
```c
	if ((error = spt_copyenv(envp)))
		goto error;

	if ((error = spt_copyargs(argc, argv)))
		goto error;
    
	SPT.nul  = nul;
	SPT.base = base;
	SPT.end  = end;

```

2. `initServerConfig();`//初始化服务器 ->     

   1. `getRandomHexChars(server.runid,REDIS_RUN_ID_SIZE);`// 设置服务器的运行 ID
   2. `server.configfile = NULL;`// 设置默认配置文件路径
   3. `server.hz = REDIS_DEFAULT_HZ;`// 设置默认服务器频率
   4. `server.runid[REDIS_RUN_ID_SIZE] = '\0';`// 为运行 ID 加上结尾字符
   5. `server.arch_bits = (sizeof(long) == 8) ? 64 : 32;`// 设置服务器的运行架构
   6. `server.port = REDIS_SERVERPORT;`// 设置默认服务器端口号
   7. `populateCommandTable();`//初始化命令行表
            
       
3. `loadServerConfig(configfile,options);` // 载入配置文件， options 是前面分析出的给定选项,这里会将argv的参数与配置文件的参数整合


4. `daemonize();`//将程序调整为守护进程


5. `initServer()`//初始化服务器数据结构 ->  
   1. 设置信号处理函数   `setupSignalHandlers`
   2. 为数据结构分配内存空间 
   3. 创建共享对象 `createSharedObjects`
   4. 初始化事件处理器 `aeCreateEventLoop`
   5. 打开UNIX本地端口 `anetUnixServer`
   6. 创建时间事件 `aeCreateTimeEvent`
   7. 创建文件事件 `aeCreateFileEvent`
   8. 打开AOF文件
   9. 初始化cluster `clusterInit();`
   10. 初始化复制功能有关的脚本缓存 `replicationScriptCacheInit();`
   11. 初始化脚本系统 `scriptingInit();`
   12. 初始化慢查询功能 `slowlogInit();`
   13. 初始化BIO系统 `bioInit();`

6. `loadDataFromDisk();` // 从 AOF 文件或者 RDB 文件中载入数据,还原数据库状态

7. `aeMain(aeEventLoop *eventLoop);` //运行事件处理器，一直到服务器关闭为止

## 一条命令生命周期
   1. 客户端发送命令
   2. 服务器读取命令，分析命令参数 `命令`->`redisClient`->`querybuf`->`argc argv`
   3. 命令执行器执行该命令实现函数，得出命令回复 `argv` -> `redisClient->cmd` -> `redisCommand proc`
   4. 回复发送给客户端

## serverCron 
* 这是 Redis 的时间中断器，每秒调用 server.hz 次。
