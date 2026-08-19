#ifndef __PORT_API_H__
#define __PORT_API_H__

typedef struct freertos_port_api_st {
    void (*vPortSetMSIPInt)(void);
    void (*vPortClearMSIPInt)(void);
    unsigned long (*taskswitch)( unsigned long sp, unsigned long arg1);
    void (*vDoTaskSwitchContext)( void );
    void (*vPortEnterCritical)( void );
    void (*vPortExitCritical)( void );
    void (*vPortClearInterruptMask)(int int_mask);
    int (*xPortSetInterruptMask)(void);
    StackType_t *(*pxPortInitialiseStack)( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters );
    // void (*prvTaskExitError)( void );
    void (*vPortSetupTimer)(void);
    void (*vPortSetupMSIP)(void);
    // void (*vPortSetup)(void);
    BaseType_t (*xPortStartScheduler)( void );
	void (*vApplicationGetIdleTaskMemory)( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
	void (*vApplicationGetTimerTaskMemory)( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );
    void (*vPortEndScheduler)( void );
    int (*xprintf)(char* format,...);
    void *(*pvPortMalloc)( size_t xWantedSize );
    void (*vPortFree)( void *pv );
}freertos_port_api_t;


extern void reg_port_func(freertos_port_api_t *freertos_port_api);

extern freertos_port_api_t g_freertos_port_api;
#define xprintf g_freertos_port_api.xprintf

#endif




