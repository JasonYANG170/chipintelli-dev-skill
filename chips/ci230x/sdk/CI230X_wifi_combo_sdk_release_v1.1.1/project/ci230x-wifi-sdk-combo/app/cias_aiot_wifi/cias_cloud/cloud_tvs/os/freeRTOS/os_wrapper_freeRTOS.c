#include "os_wrapper.h"
#include <FreeRTOS.h>

#include <semphr.h>
#include <task.h>
#include <timers.h>
#include "cias_user_config.h"
#include "os_queue.h"
long os_wrapper_get_forever_time() {
	return portMAX_DELAY;
}

#if 0
#define BUF_SIZE  512
#define MEM_SIZE_MAX   48
static void* g_test_buffer[BUF_SIZE];

static void insert_p(void* p) {
	for (int i = 0; i < BUF_SIZE; i++) {
		if (g_test_buffer[i] == NULL) {
			g_test_buffer[i] = p;
			break;
		}
	}
}

static void clear_p(void* p) {
   for (int i = 0; i < BUF_SIZE; i++) {
	   if (g_test_buffer[i] == p) {
		   g_test_buffer[i] = NULL;
		   printf("****free %p\n", p);
		   break;
	   }
   }

}

void* os_wrapper_malloc(int size, const char* file, int line) {
	void* p = malloc(size);
	if (size <= MEM_SIZE_MAX) {
		printf("****malloc %p %d %s %d\n", p, size, file, line);
		insert_p(p);
	}

	return p;
}

void* os_wrapper_realloc(void *rmem, int newsize, const char* file, int line) {
	clear_p(rmem);
	
	void* p = realloc(rmem, newsize);
	
	if (newsize <= MEM_SIZE_MAX) {
		printf("****realloc %p %d %s %d\n", p, newsize, file, line);
		insert_p(p);
	}
	
	return p;
}


void* os_wrapper_calloc(int nmemb, int size, const char* file, int line) {
	void* p =  calloc(nmemb, size);

	if (size <= MEM_SIZE_MAX) {
		printf("****calloc %p %d %s %d\n", p, size, file, line);
		insert_p(p);
	}

	return p;
}

void os_wrapper_free(void* p) {
	clear_p(p);
	if (p != NULL) {
		free(p);
	}
}

void os_wrapper_print_mem() {
	for (int i = 0; i < BUF_SIZE; i++) {
		if (g_test_buffer[i] != NULL) {
			printf("****not malloc %p\n", g_test_buffer[i]);
		}
	}
}

#endif

/***************************信号量*********************************/
void* os_wrapper_create_signal_mutex(int init_count) {
	return xSemaphoreCreateCounting(1, init_count);
}

bool os_wrapper_wait_signal(void* mutex, long time_ms) {
	return xSemaphoreTake(mutex, time_ms);
}

void os_wrapper_post_signal(void* mutex) {
	xSemaphoreGive(mutex);
}

/***************************互斥锁*********************************/
void* os_wrapper_create_locker_mutex() {
	return xSemaphoreCreateMutex();
}

bool os_wrapper_lock_mutex(void* mutex, long time_ms) {
	return xSemaphoreTake(mutex, time_ms);
}

void os_wrapper_unlock_mutex(void* mutex) {
	xSemaphoreGive(mutex);
}

void os_wrapper_sleep(long time_ms) {
	return vTaskDelay(time_ms);
}

void os_wrapper_thread_delete(void** thread_handle) {
	if (thread_handle != NULL) {
		*thread_handle = NULL;
	}
	vTaskDelete(NULL);
}

void* os_wrapper_start_thread(void* thread_func, void* param, const char* name, int prior, int stack_depth) {
	TaskHandle_t thread_handle = NULL;
	// xTaskCreate((TaskFunction_t)thread_func, name, stack_depth, param, prior+1, &thread_handle);
	xTaskCreate((TaskFunction_t)thread_func, name, stack_depth, param, prior, &thread_handle); // add by whc
	return thread_handle;
}

long os_wrapper_get_time_ms() {
	return xTaskGetTickCount();
}

void os_wrapper_start_timer(void** handle, void* func, int time_ms, bool repeat) {
	if (handle == NULL) {
		return;
	}

	if (*handle == NULL) {
		*handle = xTimerCreate("", time_ms, repeat, NULL, func);
		xTimerStart(*handle, 100);
	} else {
		xTimerChangePeriod(*handle, time_ms, 100);
	}
}

void os_wrapper_stop_timer(void* handle) {
	if (handle == NULL) {
		return;
	}
	xTimerStop(handle, 100);
}


