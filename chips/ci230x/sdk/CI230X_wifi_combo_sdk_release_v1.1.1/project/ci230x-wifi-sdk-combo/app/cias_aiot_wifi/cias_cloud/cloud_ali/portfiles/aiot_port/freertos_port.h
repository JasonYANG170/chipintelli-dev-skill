/*
 * @FileName:: 
 * @Author: liutq
 * @Date: 2022-02-11 10:38:34
 * @LastEditTime: 2022-02-11 11:56:52
 * @LastEditors: hongchuan
 * @Description: 
 */
#ifndef _FREERTOS_PORT_H_
#define _FREERTOS_PORT_H_


int32_t cias_pthread_create(TaskHandle_t *task,uint16_t stack_size,void* function,void *mqtt_handle);
void core_sysdep_sleep(uint64_t time_ms);
#define sleep core_sysdep_sleep

#endif