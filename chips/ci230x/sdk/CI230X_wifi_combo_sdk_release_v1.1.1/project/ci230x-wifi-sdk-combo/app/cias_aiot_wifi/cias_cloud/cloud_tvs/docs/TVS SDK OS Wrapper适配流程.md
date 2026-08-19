# TVS SDK OS Wrapper适配流程

os_wrapper是TVS SDK RTOS针对不同平台的系统接口不同的问题，而抽象出来的一个接口层。

接入方可以跟进当前适配的OS的特性，实现os_wrapper层接口：

- 在TVS SDK源码根目录的os目录中，新建一个文件夹，名为接入方所适配的操作系统名；
- 在该文件夹中创建一个os_wrapper.c文件；
- 修改编译脚本，将此文件加入编译文件列表中；

#### 1、内存操作接口

由于部分平台有独特的内存操作接口，比如freeRTOS可能会推荐使用pvPortMalloc/vPortFree等，rt-thread推荐使用rt_malloc和rt_free等，故在os_wrapper中进行封装接口为TVS SDK提供能力;

可以按照如下流程进入适配：

- 判断目标操作系统是否不推荐使用malloc/free等C函数，如果不推荐，在编译参数中加入

  “-DTVS_CONFIG_OS_USE_DEFAULT_MALLOC=0”，并实现os_wrapper_malloc等内存操作接
  
  口，调用系统推荐的函数；

- 如果目标操作系统推荐使用malloc/free，可以跳过本章节，如果不定义

  TVS_CONFIG_OS_USE_DEFAULT_MALLOC宏，SDK会默认将它设置为1；

- 实现分配内存块的接口：
```c
/**
 * @brief 分配内存块
 *
 * @param size 需要分配的内存块的大小，单位为字节。
 * @return 成功则返回分配的内存块地址；失败则返回NULL
 */
void* os_wrapper_malloc(int size);
```
-  实现重新分配内存块的接口；
```c
/**
 * @brief 重新分配内存块
 *
 * @param rmem	 指向已分配的内存块
 * @param newsize 重新分配的内存大小
 * @return 成功则返回分配的内存块地址；失败则返回NULL
 */
void* os_wrapper_realloc(void *rmem, int newsize);
```

- 实现分配多内存块的接口
```c
/**
 * @brief 分配多内存块
 *
 * @param nmemb	 分配的空间对象的计数
 * @param size 分配的空间对象的大小
 * @return 成功则指向指向第一个内存块地址的指针，并且所有分配的内存块都被初始化成零；如果失败则返回 NULL。
 */
void* os_wrapper_calloc(int nmemb, int size);
```

- 实现释放内存块的接口
```c
/**
 * @brief 释放内存块
 *
 * @param size 待释放的内存块指针
 * @return void
 */
void os_wrapper_free(void* p);
```

以适配rt-thread为例：

```c
#include <rtthread.h>

void* os_wrapper_malloc(int size) {
	return rt_malloc(size);
}

void* os_wrapper_realloc(void *pv, int size) {
	return rt_realloc(pv, size);
}

void* os_wrapper_calloc(int nmemb, int size) {
	return rt_calloc(nmemb, size);
}

void os_wrapper_free(void* p) {
	return rt_free(p);
}
```



#### 2、信号量

TVS SDK使用了信号量，接入方需要适配创建信号量、获取信号量和释放信号量的接口：

- 实现创建信号量的接口
```c
/**
 * @brief 创建二值信号量
 *
 * @param init_count 二值信号量的初始值，取值0或者1
 * @return 二值信号量的句柄
 */
void* os_wrapper_create_signal_mutex(int init_count);
```

- 实现信号量获取信号的接口
```c
/**
 * @brief 二值信号量等待信号
 *
 * @param mutex 二值信号量的句柄
 * @paran time_ms 等待时间，如果要永远等待下去，需要传入os_wrapper_get_forever_time的返回值
 * @return void
 */
```

- 实现信号量释放信号的接口
```c
/**
 * @brief 二值信号量释放信号
 *
 * @param mutex 二值信号量的句柄
 * @return void
 */
void os_wrapper_post_signal(void* mutex);
```

- 实现持续等待事件的接口
```c
/**
 * @brief 在信号量等待信号，或者互斥量加锁的时候，如果要持续等待到获得信号或者加锁成功为止，需要调用此函数，并将结果作为参数传入对应函数中；

 * 例如：os_wrapper_wait_signal(mutex, os_wrapper_get_forever_time());
 *
 
 * @param 

 * @return 二值信号量的句柄
 */
long os_wrapper_get_forever_time();
```



以适配rt-thread为例：

```c
long os_wrapper_get_forever_time() {
	return RT_WAITING_FOREVER;
}

void* os_wrapper_create_signal_mutex(int init_count){
	return rt_sem_create("tvs", init_count, RT_IPC_FLAG_FIFO);
}

bool os_wrapper_wait_signal(void* mutex, long time_ms) {
	return rt_sem_take(mutex, time_ms) == RT_EOK;
}

void os_wrapper_post_signal(void* mutex) {
	rt_sem_release(mutex);
}
```



#### 3、互斥量

- 创建互斥量
```c
/**
 * @brief 创建互斥量，一般用于保护公共变量，处理线程安全问题
 *
 * @param
 * @return 互斥量的句柄
 */
void* os_wrapper_create_locker_mutex();
```

- 互斥量加锁
```c
/**
* @brief 互斥量加锁
*
* @param mutex 目标互斥量的句柄
* @paran time_ms 等待时间，如果要永远等待下去，需要传入
*                os_wrapper_get_forever_time的返回值
* @return 为true代表加锁成功，为false代表失败或者超时
*/
bool os_wrapper_lock_mutex(void* mutex, long time_ms);
```
- 互斥量解锁
```c
/**
 * @brief 互斥量解锁
 *
 * @param mutex 目标互斥量
 * @return void
 */
void os_wrapper_unlock_mutex(void* mutex);
```



以适配rt-thread为例：

```c
void* os_wrapper_create_signal_mutex(int init_count){
	return rt_sem_create("tvs", init_count, RT_IPC_FLAG_FIFO);
}

bool os_wrapper_wait_signal(void* mutex, long time_ms) {
	return rt_sem_take(mutex, time_ms) == RT_EOK;
}

void os_wrapper_post_signal(void* mutex) {
	rt_sem_release(mutex);
}
```



#### 4、启动线程

- 实现启动线程的接口
```c
/**
 * @brief 启动线程
 *
 * @param thread_func 线程函数，void ()(void*)
 * @param param 线程参数
 * @param name 线程名称
 * @param prior 线程优先级，数值越大优先级越高
 * @param stack_depth 线程栈深度，注意，单位为字（4bytes）
 * @return 线程的句柄，NULL代表启动线程失败
 */
void* os_wrapper_start_thread(void* thread_func, void* param, const char* name, int prior, int stack_depth);
```

- 实现删除线程的接口
```c
/**
 * @brief 此函数一般用于freeRTOS, 在task末尾调用vTaskDelete(NULL),其他OS一般用不到
 *
 * @param thread_handle 线程句柄
 * @return void
 */
void os_wrapper_thread_delete(void** thread_handle);
```


以适配rt-thread为例：

```c
void os_wrapper_thread_delete(void** thread_handle) {
	// rt-thread无需实现此函数
}

void* os_wrapper_start_thread(void* thread_func, void* param, const char* name, int prior, int stack_depth) {
	rt_thread_t tid = RT_NULL;

	tid = rt_thread_create(name, thread_func, param, stack_depth * 4, prior + 20, THREAD_TIMESLICE);

	if (tid != RT_NULL) {
		rt_thread_startup(tid);
	}

	return tid;
}
```

#### 5、系统时钟

- 实现获取系统时钟的接口
```c
/**
 * @brief 获取从开机到当前时刻的持续时间
 *
 * @param 
 * @return 从开机到当前时刻的持续时间，单位为毫秒
 */
long os_wrapper_get_time_ms();
```

以适配rt-thread为例：

```c
long os_wrapper_get_time_ms() {
	return rt_tick_get();
}
```



#### 6、定时器

- 实现启动定时器的接口
```c
/**
 * @brief 启动定时器
 *
 * @param handle 出参，timer的句柄
 * @param func 定时器触发时执行的函数
 * @param time_ms 定时间隔，单位为毫秒
 * @param repeat true代表定时器将重复触发，false代表只触发一次
 * @return void
 */
```

- 实现停止定时器的接口
```c
/**
 * @brief 停止定时器
 *
 * @param handle timer的句柄
 * @return void
 */
```

以适配rt-thread为例：
```c
void os_wrapper_start_timer(void** handle, void* func, int time_ms, bool repeat) {
	if (handle == NULL) {
		return;
	}

	if (*handle != NULL) {
		rt_timer_stop(*handle);
		rt_timer_delete(*handle);
	}
	
	rt_uint8_t flag = repeat ? RT_TIMER_FLAG_PERIODIC : RT_TIMER_FLAG_ONE_SHOT;
	
	*handle = rt_timer_create("t", func, RT_NULL, time_ms, flag);
	
	if (*handle != RT_NULL) {
		rt_timer_start(*handle);
	}
}

void os_wrapper_stop_timer(void* handle) {
	if (handle == NULL) {
		return;
	}

	rt_timer_stop(handle);

}
```