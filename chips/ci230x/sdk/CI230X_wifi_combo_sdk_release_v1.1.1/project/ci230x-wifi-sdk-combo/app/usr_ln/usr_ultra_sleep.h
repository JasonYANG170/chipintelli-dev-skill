#ifndef __USR_ULTRA_SLEEP_H__
#define __USR_ULTRA_SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif

//如需ultra sleep功能，打开此宏ULTRA_LOW_POWER_SLEEP即可
#define ULTRA_LOW_POWER_SLEEP 

#ifdef ULTRA_LOW_POWER_SLEEP
    #define EXTIO_WAKEUP_IO_PORT        (GPIOA_BASE)
    #define EXTIO_WAKEUP_IO_PIN         (GPIO_PIN_6)
    #define EXTIO_WAKEUP_IO_PULL        (GPIO_PULL_UP)  
    #define EXTIO_INT_SENSE             (EXT_INT_SENSE_5) 
    #define EXTIO_INT_TRIG_TYPE         (EXT_INT_NEGEDGE) 
    #define EXTIO_INT_PRIO              (1)  
    #define EXTIO_INT_SENSE_IT_FLAG     (EXT_INT_SENSE_5_IT_FLAG)

    extern void ultra_lowpower_enter(void);
    
#endif

#ifdef __cplusplus
}
#endif

#endif /* __USR_APP_H__ */
