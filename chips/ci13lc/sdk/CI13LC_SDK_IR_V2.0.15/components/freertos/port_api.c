#include "FreeRTOS.h"
#include "../../system/port_api.h"


freertos_port_api_t g_freertos_port_api = {0};


void reg_port_func(freertos_port_api_t *freertos_port_api) 
{
    g_freertos_port_api = *freertos_port_api;
}

void vPortSetMSIPInt(void)
{
    g_freertos_port_api.vPortSetMSIPInt();
}

void vPortClearMSIPInt(void)
{
    g_freertos_port_api.vPortClearMSIPInt();
}

unsigned long taskswitch( unsigned long sp, unsigned long arg1)
{
    return g_freertos_port_api.taskswitch(sp, arg1);
}

void vDoTaskSwitchContext( void )
{
    g_freertos_port_api.vDoTaskSwitchContext();
}

void vPortEnterCritical( void )
{
    g_freertos_port_api.vPortEnterCritical();
}

void vPortExitCritical( void )
{
    g_freertos_port_api.vPortExitCritical();
}

void vPortClearInterruptMask(int int_mask)
{
    g_freertos_port_api.vPortClearInterruptMask(int_mask);
}

int xPortSetInterruptMask(void)
{
    return g_freertos_port_api.xPortSetInterruptMask();
}

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    StackType_t *rst =  g_freertos_port_api.pxPortInitialiseStack(pxTopOfStack, pxCode, pvParameters);
    return rst;
}

// void prvTaskExitError( void )
// {
//     g_freertos_port_api.prvTaskExitError();
// }

void vPortSetupTimer(void)
{
    g_freertos_port_api.vPortSetupTimer();
}

void vPortSetupMSIP(void)
{
    g_freertos_port_api.vPortSetupMSIP();
}

// void vPortSetup(void)
// {
//     g_freertos_port_api.vPortSetup();
// }

BaseType_t xPortStartScheduler( void )
{
   return g_freertos_port_api.xPortStartScheduler();
}

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    g_freertos_port_api.vApplicationGetIdleTaskMemory(ppxIdleTaskTCBBuffer, ppxIdleTaskStackBuffer, pulIdleTaskStackSize);
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    g_freertos_port_api.vApplicationGetTimerTaskMemory(ppxTimerTaskTCBBuffer, ppxTimerTaskStackBuffer, pulTimerTaskStackSize);
}

void vPortEndScheduler( void )
{
    g_freertos_port_api.vPortEndScheduler();
}


void *pvPortMalloc( size_t xWantedSize )
{
    return g_freertos_port_api.pvPortMalloc(xWantedSize);
}

void vPortFree( void *pv )
{
    g_freertos_port_api.vPortFree(pv);
}


