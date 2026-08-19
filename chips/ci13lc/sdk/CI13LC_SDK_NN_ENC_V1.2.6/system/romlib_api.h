#ifndef __ROMLIB_API_H__
#define __ROMLIB_API_H__

#define USE_FFT    1
#define USE_CLIB   1

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "stream_buffer.h"

#include "../../system/port_api.h"
#include "fft.h"
#include "../../utils/dichotomy_find.h"




typedef struct romruntime_func_st {
	char ci_lib_romruntime[8];
	uint32_t verison;
	uint32_t reserved;
	int (*init_lib_romruntime_p)(void);
    #if USE_FFT
	struct fftfunc_s {
		void (*riscv_rfft_fast_f32_p)(riscv_rfft_fast_instance_f32 * S,	float * p,float * pOut,uint8_t ifftFlag);
		int (*riscv_rfft_fast_init_f32_p)( riscv_rfft_fast_instance_f32 * S,unsigned int fft_len);
	}fftfunc;
	#endif /*USE_FFT*/
	#if USE_CLIB
	struct newlibcfunc_s {
		void 	    (*NEWLib_Set_Func_p)(void* f1,void* f2,void* f3);
		int 	    (*memcmp_p)(const void *, const void *, size_t);
		void *      (*memcpy_p)(void *__restrict, const void *__restrict, size_t);
		void *	    (*memmove_p)(void *, const void *, size_t);
		void *	    (*memset_p)(void *, int, size_t);
		char *      (*strcat_p)(char *__restrict, const char *__restrict);
		int	        (*strcmp_p)(const char *, const char *);
		char *      (*strcpy_p)(char *__restrict, const char *__restrict);
		size_t	    (*strlen_p)(const char *);
		char *      (*strncat_p)(char *__restrict, const char *__restrict, size_t);
		int	        (*strncmp_p)(const char *, const char *, size_t);
		char *      (*strncpy_p)(char *__restrict, const char *__restrict, size_t);
		char *      (*strstr_p)(const char *, const char *);
		char *      (*stpcpy_p)(char *__restrict, const char *__restrict);
		char *      (*stpncpy_p)(char *__restrict, const char *__restrict, size_t);
		size_t	    (*strnlen_p)(const char *, size_t);
		unsigned long (*strtoul_p)(const char *__restrict __n, char **__restrict __end_PTR, int __base);

		long	    (*atol_p)(const char *__nptr);
		void	    (*qsort_p)(void *__base, size_t __nmemb, size_t __size, __compar_fn_t _compar);

		void *	    (*malloc_p)(size_t __size);
		void	    (*free_p)(void *);
        void *      (*_malloc_r_p)(struct _reent *p, size_t);
        void        (*_free_r_p)(struct _reent *p, void *);

		int	        (*vsnprintf_p)(char *__restrict, size_t, const char *__restrict, __VALIST);
		int	        (*sprintf_p) (char *__restrict, const char *__restrict, ...);
		int			(*sscanf_p)(const char *__restrict, const char *__restrict, ...);


		int	  (*abs_p)(int);
		int   (*isnan_p)(double);
		int   (*isinf_p)(double);
		float (*cosf_p)(float);
		float (*sinf_p)(float);
		float (*tanf_p)(float);
		float (*expf_p)(float);
		float (*sqrtf_p)(float);
		float (*fabsf_p)(float);
		float (*logf_p)(float);
		float (*log10f_p)(float);
	}newlibcfunc;
	#endif /*USE_CLIB*/

    struct freertos_api_st {
        void (*reg_port_func)(freertos_port_api_t *freertos_port_api);
        BaseType_t (*xTaskCreate)(	TaskFunction_t pxTaskCode,
		    const char * const pcName,	/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
		    const configSTACK_DEPTH_TYPE usStackDepth,
		    void * const pvParameters,
		    UBaseType_t uxPriority,
		    TaskHandle_t * const pxCreatedTask );
        TaskHandle_t (*xTaskCreateStatic)(TaskFunction_t pxTaskCode, 
            const char* const pcName, 
            const uint32_t ulStackDepth, 
            void * const pvParameters, 
            UBaseType_t uxPriority, 
            StackType_t * const puxStackBuffer,
            StaticTask_t * const pxTaskBuffer);
        void (*vTaskDelete)(TaskHandle_t xTaskToDelete);
        void (*vTaskDelay)(const TickType_t xTicksToDelay);
        void (*vTaskDelayUntil)(TickType_t * const pxPreviousWakeTime, const TickType_t xTimeIncrement);
        UBaseType_t (*uxTaskPriorityGet)( const TaskHandle_t xTask);
        UBaseType_t (*uxTaskPriorityGetFromISR)( const TaskHandle_t xTask);
        eTaskState (*eTaskGetState)(TaskHandle_t xTask);
        void (*vTaskGetInfo)(TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetFreeStackSpace, eTaskState eState);
        void (*vTaskPrioritySet)(TaskHandle_t xTask, UBaseType_t uxNewPriority);
        void (*vTaskSuspend)(TaskHandle_t xTaskToSuspend);
        void (*vTaskResume)(TaskHandle_t xTaskToResume);
        BaseType_t (*xTaskResumeFromISR)(TaskHandle_t xTaskToResume);
        void (*vTaskStartScheduler)(void);
        void (*vTaskEndScheduler)(void);
        void (*vTaskSuspendAll)(void);
        BaseType_t (*xTaskResumeAll)(void);
        TickType_t (*xTaskGetTickCount)(void);
        TickType_t (*xTaskGetTickCountFromISR)(void);
        UBaseType_t (*uxTaskGetNumberOfTasks)(void);
        char *(*pcTaskGetName)(TaskHandle_t xTaskToQuery);
        TaskHandle_t (*xTaskGetHandle)(const char *pcNameToQuery);
        UBaseType_t (*uxTaskGetStackHighWaterMark)(TaskHandle_t xTask);
        UBaseType_t (*uxTaskGetSystemState)(TaskStatus_t * const pxTaskStatusArray, const UBaseType_t uxArraySize, uint32_t* const pulTotalRunTime);
        BaseType_t (*xTaskGenericNotify)(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue);
        BaseType_t (*xTaskGenericNotifyFromISR)(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue, BaseType_t *pxHigherPriorityTaskWoken);
        BaseType_t (*xTaskNotifyWait)(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait);
        void (*vTaskNotifyGiveFromISR)(TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken);
        uint32_t (*ulTaskNotifyTake)(BaseType_t xClearCountOnExit, TickType_t xTicksToWait);
        BaseType_t (*xTaskNotifyStateClear)(TaskHandle_t xTask);
        BaseType_t (*xTaskIncrementTick)(void);
        void (*vTaskSwitchContext)(void);
        TaskHandle_t (*xTaskGetCurrentTaskHandle)(void);
        BaseType_t (*xTaskGetSchedulerState)(void);
        UBaseType_t (*uxTaskGetTaskNumber)(TaskHandle_t xTask);
        void (*vTaskSetTaskNumber)(TaskHandle_t xTask, const UBaseType_t uxHandle);
        EventGroupHandle_t (*xEventGroupCreate)( void );
        EventGroupHandle_t (*xEventGroupCreateStatic)(StaticEventGroup_t *pxEventGroupBuffer);
        EventBits_t (*xEventGroupWaitBits)(EventGroupHandle_t, const EventBits_t, const BaseType_t, const BaseType_t, TickType_t);
        EventBits_t (*xEventGroupClearBits)(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);
        BaseType_t (*xEventGroupClearBitsFromISR)(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear);
        EventBits_t (*xEventGroupSetBits)(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet);
        BaseType_t (*xEventGroupSetBitsFromISR)(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken);
        EventBits_t (*xEventGroupSync)(EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait);
        EventBits_t (*xEventGroupGetBitsFromISR)(EventGroupHandle_t xEventGroup);
        void (*vEventGroupDelete)(EventGroupHandle_t xEventGroup);
        UBaseType_t (*uxEventGroupGetNumber)(void* xEventGroup);
        void (*vEventGroupSetNumber)(void* xEventGroup, UBaseType_t uxEventGroupNumber);
        void (*vListInitialise)(List_t * const pxList );
        void (*vListInitialiseItem)(ListItem_t * const pxItem);
        void (*vListInsert)(List_t * const pxList, ListItem_t * const pxNewListItem);
        void (*vListInsertEnd)(List_t * const pxList, ListItem_t * const pxNewListItem);
        UBaseType_t (*uxListRemove)(ListItem_t * const pxItemToRemove);
        BaseType_t (*xQueueGenericReset)(QueueHandle_t xQueue, BaseType_t xNewQueue);
	    QueueHandle_t (*xQueueGenericCreate)( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType );
        QueueHandle_t (*xQueueGenericCreateStatic)( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, uint8_t *pucQueueStorage, StaticQueue_t *pxStaticQueue, const uint8_t ucQueueType);
        QueueHandle_t (*xQueueCreateMutex)( const uint8_t ucQueueType );
	    QueueHandle_t (*xQueueCreateMutexStatic)( const uint8_t ucQueueType, StaticQueue_t *pxStaticQueue );
        QueueHandle_t (*xQueueCreateCountingSemaphore)( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount );
	    QueueHandle_t (*xQueueCreateCountingSemaphoreStatic)( const UBaseType_t uxMaxCount, const UBaseType_t uxInitialCount, StaticQueue_t *pxStaticQueue );
        BaseType_t (*xQueueGenericSend)( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition );
        BaseType_t (*xQueueGenericSendFromISR)( QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition );
        BaseType_t (*xQueueGiveFromISR)( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken );
        BaseType_t (*xQueueReceive)( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait );
        BaseType_t (*xQueueSemaphoreTake)( QueueHandle_t xQueue, TickType_t xTicksToWait );
        BaseType_t (*xQueuePeek)( QueueHandle_t xQueue, void * const pvBuffer, TickType_t xTicksToWait );
        BaseType_t (*xQueueReceiveFromISR)( QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken );
        BaseType_t (*xQueuePeekFromISR)( QueueHandle_t xQueue,  void * const pvBuffer );
        UBaseType_t (*uxQueueMessagesWaiting)( const QueueHandle_t xQueue );
        UBaseType_t (*uxQueueSpacesAvailable)( const QueueHandle_t xQueue );
        UBaseType_t (*uxQueueMessagesWaitingFromISR)( const QueueHandle_t xQueue );
        void (*vQueueDelete)( QueueHandle_t xQueue );
	    UBaseType_t (*uxQueueGetQueueNumber)( QueueHandle_t xQueue );
	    void (*vQueueSetQueueNumber)( QueueHandle_t xQueue, UBaseType_t uxQueueNumber );
	    uint8_t (*ucQueueGetQueueType)( QueueHandle_t xQueue );
        BaseType_t (*xQueueIsQueueEmptyFromISR)( const QueueHandle_t xQueue );
        BaseType_t (*xQueueIsQueueFullFromISR)( const QueueHandle_t xQueue );
	    void (*vQueueAddToRegistry)( QueueHandle_t xQueue, const char *pcQueueName );
	    const char *(*pcQueueGetName)( QueueHandle_t xQueue );
	    void (*vQueueUnregisterQueue)( QueueHandle_t xQueue );
	    void (*vQueueWaitForMessageRestricted)( QueueHandle_t xQueue, TickType_t xTicksToWait, const BaseType_t xWaitIndefinitely );
        BaseType_t (*xQueueGenericIsFull)(QueueHandle_t xQueue);
        BaseType_t (*xQueueGenericIsEmpty)(QueueHandle_t xQueue);
        StreamBufferHandle_t (*xStreamBufferGenericCreate)( size_t xBufferSizeBytes,
												 size_t xTriggerLevelBytes,
												 BaseType_t xIsMessageBuffer );
        StreamBufferHandle_t (*xStreamBufferGenericCreateStatic)( size_t xBufferSizeBytes,
														   size_t xTriggerLevelBytes,
														   BaseType_t xIsMessageBuffer,
														   uint8_t * const pucStreamBufferStorageArea,
														   StaticStreamBuffer_t * const pxStaticStreamBuffer );
        void (*vStreamBufferDelete)( StreamBufferHandle_t xStreamBuffer );
        BaseType_t (*xStreamBufferReset)( StreamBufferHandle_t xStreamBuffer );
        BaseType_t (*xStreamBufferSetTriggerLevel)( StreamBufferHandle_t xStreamBuffer, size_t xTriggerLevel );
        size_t (*xStreamBufferSpacesAvailable)( StreamBufferHandle_t xStreamBuffer );
        size_t (*xStreamBufferBytesAvailable)( StreamBufferHandle_t xStreamBuffer );
        size_t (*xStreamBufferSend)( StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t xDataLengthBytes, TickType_t xTicksToWait );
        size_t (*xStreamBufferSendFromISR)( StreamBufferHandle_t xStreamBuffer, const void *pvTxData, size_t xDataLengthBytes, BaseType_t * const pxHigherPriorityTaskWoken );
        size_t (*xStreamBufferReceive)( StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t xBufferLengthBytes, TickType_t xTicksToWait );
        size_t (*xStreamBufferNextMessageLengthBytes)( StreamBufferHandle_t xStreamBuffer );
        size_t (*xStreamBufferReceiveFromISR)( StreamBufferHandle_t xStreamBuffer, void *pvRxData, size_t xBufferLengthBytes, BaseType_t * const pxHigherPriorityTaskWoken );
        BaseType_t (*xStreamBufferIsEmpty)( StreamBufferHandle_t xStreamBuffer );
        BaseType_t (*xStreamBufferIsFull)( StreamBufferHandle_t xStreamBuffer );
        BaseType_t (*xStreamBufferSendCompletedFromISR)( StreamBufferHandle_t xStreamBuffer, BaseType_t *pxHigherPriorityTaskWoken );
        BaseType_t (*xStreamBufferReceiveCompletedFromISR)( StreamBufferHandle_t xStreamBuffer, BaseType_t *pxHigherPriorityTaskWoken );
	    UBaseType_t (*uxStreamBufferGetStreamBufferNumber)( StreamBufferHandle_t xStreamBuffer );
	    void (*vStreamBufferSetStreamBufferNumber)( StreamBufferHandle_t xStreamBuffer, UBaseType_t uxStreamBufferNumber );
	    uint8_t (*ucStreamBufferGetStreamBufferType)( StreamBufferHandle_t xStreamBuffer );
        TimerHandle_t (*xTimerCreate)(	const char * const pcTimerName,			/*lint !e971 Unqualified char types are allowed for strings and single characters only. */
								const TickType_t xTimerPeriodInTicks,
								const UBaseType_t uxAutoReload,
								void * const pvTimerID,
								TimerCallbackFunction_t pxCallbackFunction );
        TimerHandle_t (*xTimerCreateStatic)(const char * const pcTimerName,
										const TickType_t xTimerPeriodInTicks,
										const UBaseType_t uxAutoReload,
										void * const pvTimerID,
										TimerCallbackFunction_t pxCallbackFunction,
										StaticTimer_t *pxTimerBuffer );
        void *(*pvTimerGetTimerID)( const TimerHandle_t xTimer );
        void (*vTimerSetTimerID)( TimerHandle_t xTimer, void *pvNewID );
        BaseType_t (*xTimerIsTimerActive)( TimerHandle_t xTimer );
        TaskHandle_t (*xTimerGetTimerDaemonTaskHandle)( void );
        BaseType_t (*xTimerPendFunctionCallFromISR)( PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, BaseType_t *pxHigherPriorityTaskWoken );
        BaseType_t (*xTimerPendFunctionCall)( PendedFunction_t xFunctionToPend, void *pvParameter1, uint32_t ulParameter2, TickType_t xTicksToWait );
        const char * (*pcTimerGetName)( TimerHandle_t xTimer );
        TickType_t (*xTimerGetPeriod)( TimerHandle_t xTimer );
        TickType_t (*xTimerGetExpiryTime)( TimerHandle_t xTimer );
        BaseType_t (*xTimerGenericCommand)( TimerHandle_t xTimer, const BaseType_t xCommandID, const TickType_t xOptionalValue, BaseType_t * const pxHigherPriorityTaskWoken, const TickType_t xTicksToWait );
	    void (*vTimerSetTimerNumber)( TimerHandle_t xTimer, UBaseType_t uxTimerNumber );
	    UBaseType_t (*uxTimerGetTimerNumber)( TimerHandle_t xTimer );
    }freertos_api;
    
    struct utils_st {
        uint8_t (*crc8)(uint8_t pre_crc, const uint8_t * data, uint32_t length);
        uint16_t (*crc16_ccitt)(uint16_t pre_crc, const uint8_t * data, uint32_t length);
        uint32_t (*crc32)(uint32_t pre_crc, const uint8_t * data, uint32_t length);
        int (*dichotomy_find)(void *pValue, int MinIndex, int MaxIndex,COMPARE_CALLBACK CompareFunc,void *CallbackPara);
    }utils;

    struct data_table_st {
        const uint8_t mel_scale[0x800];             // float[]
        const uint8_t mel_offset_size[0x78];        // short[]
        const uint8_t asr_windowfun[0x320];         // short[]
        const uint16_t StepSizeTable[89];           // uint16_t[] 
    }data_table;
}romruntime_func_t;


#define MASK_ROM_LIB_RUNTIME_ADDR    (0x1F000000+10*1024)

#define MASK_ROM_LIB_FUNC            ((romruntime_func_t* )MASK_ROM_LIB_RUNTIME_ADDR)
#define MASK_ROM_LIB_FFT_FUNC        (((romruntime_func_t* )MASK_ROM_LIB_RUNTIME_ADDR)->fftfunc)
#define MASK_ROM_LIB_C_FUNC          (((romruntime_func_t* )MASK_ROM_LIB_RUNTIME_ADDR)->newlibcfunc)
#define MASK_ROM_LIB_FREERTOS_FUNC   (((romruntime_func_t* )MASK_ROM_LIB_RUNTIME_ADDR)->freertos_api)
#define MASK_ROM_LIB_UTIL_FUNC       (((romruntime_func_t* )MASK_ROM_LIB_RUNTIME_ADDR)->utils)
#define MASK_ROM_LIB_TABLE           (((romruntime_func_t* )MASK_ROM_LIB_RUNTIME_ADDR)->data_table)



#define MASK_ROM_BOOT_CODE_ADDR      (0x1F000000)    
#define MASK_ROM_CORE_MAGIC_NUM_ADDR (0x1F000000+0x1C
#define MASK_ROM_WINDOW_ADDR         (0x1F000000+79*1024)                              // sin窗函数
#define MASK_ROM_FFT_BIT_ADDR        (0x1F000000+8*1024)                               // 512fft的表1
#define MASK_ROM_FFT_COEF_R_ADDR     (MASK_ROM_FFT_BIT_ADDR + sizeof(short)*440)       // 512fft的表2
#define MASK_ROM_FFT_COEF_ADDR       (MASK_ROM_FFT_COEF_R_ADDR + sizeof(float)*512)    // 512fft的表3
#define MASK_ROM_MEL_SCALE_ADDR      (MASK_ROM_FFT_COEF_ADDR + sizeof(float)*512)      // mel需要的表1
#define MASK_ROM_MEL_OFFSET_ADDR     (MASK_ROM_MEL_SCALE_ADDR + sizeof(float)*512)     // mel需要的表2
#define MASK_ROM_ASR_WINDOW_ADDR     (MASK_ROM_MEL_OFFSET_ADDR + sizeof(short)*(60))   // fe 需要的窗函数
#define MASK_ROM_PCM_TABLE_ADDR      (MASK_ROM_ASR_WINDOW_ADDR + sizeof(short)*(400))  // ADPCM需要用的表

#define RISCVBITREVINDEXTABLE_256_TABLE_LENGTH_TEST (440)
#define RISCVBITREVINDEXTABLE_512_TABLE_LENGTH_TEST (448)
#endif



