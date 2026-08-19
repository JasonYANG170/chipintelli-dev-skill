/*============================================================================*/
/* @file exe_port.h
 * @brief  The micro for porting to different cpu
 * @author onmicro
 * @date 2020/03
 */

#ifndef __EXE_PORT_H__
#define __EXE_PORT_H__


#if defined(STM32F103)

	#if   defined ( __CC_ARM )
	  #define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
	  #define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
	  #define __STATIC_INLINE  static __inline
	  #define __PACKED         __packed
	  #define __PACKED_GCC

	#elif defined ( __ICCARM__ )
	  #define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
	  #define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
	  #define __STATIC_INLINE  static inline
	  #define __PACKED         __packed
	  #define __PACKED_GCC

	#elif defined ( __TMS470__ )
	  #define __ASM            __asm                                      /*!< asm keyword for TI CCS Compiler       */
	  #define __STATIC_INLINE  static inline

	#elif defined ( __GNUC__ )
	  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
	  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
	  #define __STATIC_INLINE  static inline
	  #define __PACKED
	  #define __PACKED_GCC     __attribute__ ((__packed__))

	#elif defined ( __TASKING__ )
	  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
	  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */
	  #define __STATIC_INLINE  static inline
	  #define __PACKED
	  #define __PACKED_GCC     __attribute__ ((__packed__))

	#endif
		
	#define __ONCHIP_CODE__
	
#elif defined(HS6601) || defined(HS6601c)

#else
#define HAL_STORAGE_CFG_BASE            0x9000
#endif


#endif /* #ifndef __EXE_PORT_H__ */

