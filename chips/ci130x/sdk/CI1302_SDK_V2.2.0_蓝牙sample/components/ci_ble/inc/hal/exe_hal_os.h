/*============================================================================*/
/* @file exe_hal_os.h
 * @brief EXE HAL OS header.
 * @author onmicro
 * @date 2020/02
 */

#ifndef __EXE_HAL_OS_H__
#define __EXE_HAL_OS_H__

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#if HAL_RTOS_SUPPORT
/* cmsis RTOS version */
#include "cmsis_os2.h"

#define EXE_IRQ_SAVE()                 do { uint32_t __isrm = ostaskENTER_CRITICAL_FROM_ISR();
#define EXE_IRQ_RESTORE()              ostaskEXIT_CRITICAL_FROM_ISR(__isrm); } while (0)

#elif defined(HS66XX)
/* nonOS */
#define ostaskENTER_CRITICAL()
#define ostaskEXIT_CRITICAL()

/* cmsis raw register version */
#define EXE_IRQ_SAVE()                  do { uint32_t __irq_save = __get_PRIMASK(); __set_PRIMASK(1);
#define EXE_IRQ_RESTORE()               __set_PRIMASK(__irq_save); } while(0)

#elif defined(HM1001_M0)
/* nonOS + wheel isr */
void hal_os_cr_enter(void);
void hal_os_cr_exit(void);
#define ostaskENTER_CRITICAL()          hal_os_cr_enter()
#define ostaskEXIT_CRITICAL()           hal_os_cr_exit()

/* cmsis raw register version */
#define EXE_IRQ_SAVE()                  do { uint32_t __irq_save = __get_PRIMASK(); __set_PRIMASK(1);
#define EXE_IRQ_RESTORE()               __set_PRIMASK(__irq_save); } while(0)

#else
/* nonOS */
#define ostaskENTER_CRITICAL() vTaskSuspendAll()
#define ostaskEXIT_CRITICAL() xTaskResumeAll()

/* dummy version */
#define EXE_IRQ_SAVE()
#define EXE_IRQ_RESTORE()
#endif

#endif /* #ifndef __EXE_HAL_OS_H__ */
