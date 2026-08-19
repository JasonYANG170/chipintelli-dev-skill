#include "cias_common.h"
#include "hal/hal_gpio.h"
#include "utils/reboot_trace/reboot_trace.h"
void cias_heap_info()
{
    UBaseType_t ArraySize;
    TaskStatus_t *StatusArray;
    ArraySize = uxTaskGetNumberOfTasks();
    StatusArray = pvPortMalloc(ArraySize * sizeof(TaskStatus_t));
    if (StatusArray && ArraySize)
    {
        uint32_t ulTotalRunTime;
        volatile UBaseType_t ArraySize2 = uxTaskGetSystemState(StatusArray, ArraySize, &ulTotalRunTime);
        CIAS_LOG_HL("TaskName\t\tPriority\tTaskNumber\tMinStk\t%d\n", ArraySize2);
        for (int i = 0; i < ArraySize2; i++)
        {
            CIAS_LOG_DEBUG2("% -16s\t%d\t\t%d\t\t%d\r\n",
                            StatusArray[i].pcTaskName,
                            (int)StatusArray[i].uxCurrentPriority,
                            (int)StatusArray[i].xTaskNumber,
                            (int)StatusArray[i].usStackHighWaterMark);
        }
        CIAS_LOG_HL("\n");
        CIAS_LOG_HL("********>>>>>Heap left: %d <> min:%d<<<<<*********\n", xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
    }
    vPortFree(StatusArray);
}
void cias_system_reboot(void)
{
    ln_chip_reboot();
}





