/*
  2019.10.14, LT, ffthw_ifft_polling_compute_half_window512_256, use 257 complexs for inuput
  2019.08.02, LT, Add hardware *W256_s128*;
  2019.07.10, LT, More APIs, add some relation files;
  2019.07.03, LT, Discard the ci_fft_* APIs, softwareFFT and hardwareFFT are standlone now;
  2019.06.13, LT, Use marco USE_HARDWARE_FFT instead of NO_HARDWARE_FFT;
  2019.04.18, LT, Add api ci_fft_development_version();
  2019.04.04, LT, Check the user's args when using ci_fft_fft() and ci_fft_ifft(); 
  2019.03.23, LT, Add ci_fft_fft() and ci_fft_ifft for hardwarefft of softwarefft,
                     software_fft or hardware_fft, not both;
  2019.03.15, LT, Use marcos for *window512_256 or *window400_160 depends on FRAME_SIZE;
  2019.03.13, LT, Use *window512_256 or *window400_160 for windowsize_windowshiftsize;
  2019.02.28, LT, FFT and IFFT support frame_size=400, fft_size=256 only;
  2019.01.15, LT, Add fft's testcode;
  2019.01.09, LT, Rename files as ci_fft.*;
                     We don't use s_fft.tmp_data because it causes unreenterable API;
                     Add API ci_fft_version();
  2019.01.08, LT, Use ffthw_ifft_polling_compute_half() on FPGA_v5,
                     (we must set fft_psd_rslt_ptr_addr, or be error);
                     Use macro MPW_FLATFORM
  2018.12.10, LT, Add the software fft, just for test hardware fft.
  2018.11.29, LT, Create.
***************************************************************************************/

//#include "../../printf/ci_log.h"
//#include "riscv_math.h"
//#include "ci_fft.h"
#include "ci_system.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "romlib_api.h"
//#include "rv_fft.h"


/* resource
*****************
    codesize:  ~= 1.5 KBytes hardware_fft
               ~= 7.5 KBytes software_fft, rodata ~= 1K Bytes, rwdata ~=0.1K Bytes
    time:     ? ms 80MHz  V7 hardwareFFT;  ? Mcycles
              ? ms 168MHz mpw softwareFFT; ? Mcycles
    stack:    ~= 0.5 KBytes(hardwareFFT) / ~= 2.1 KBytes(softwareFFT)
****************/


/* macros
***************************************************************************************/

// the version is 1.00.00
#define CI_FFT_VERSION  10000


#define NFFT 512
#define FRAME_SIZE (512)

static float fft_buf_tmp[1024];

/* structions
***************************************************************************************/
typedef struct
{
    volatile int initing;
    int inited;
    int hardware_fft_inited;
} fft_s;


/* global vars. definitions
***************************************************************************************/

 riscv_rfft_fast_instance_f32 S = {0};
//static riscv_rfft_fast_1024_instance_f32 S_1024 = {0};

SemaphoreHandle_t fft_xSemaphore = NULL;

/* function definitions
***************************************************************************************/



/****************************************************************************
 * read the *.h file please
 ****************************************************************************/
int ci_fft_version( void )
{
    return CI_FFT_VERSION;
}

int ci_software_fft_w512_s256_init( void )
{
	static char sft_fft_init = 0;
	if(!sft_fft_init)
	{
		fft_xSemaphore = xSemaphoreCreateMutex();
		if(NULL == fft_xSemaphore)
		{
			return -1;
		}
		//riscv_rfft_512_fast_init_f32( &S );
        MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_init_f32_p(&S,512);
		sft_fft_init = 1;	
	}
    return 0;
}


int ci_software_fft_w1024_s512_init( void )
{
	//_riscv_rfft_1024_fast_init_f32( &S_1024) ;
    MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_init_f32_p(&S,1024);
    return RET_SUCCESS;
}

int ci_software_ifft_w1024_s512(const float *fft, float *result)
{
	//_riscv_rfft_1024_fast_f32( &S_1024, fft, result, 1);
    MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_f32_p(&S, (float*)fft,result,1);
	return RET_SUCCESS;
}

int ci_software_fft_w1024_s512( const short *audio_data, float *window_data, float *result )
{
    int i = 0;
    short* fix_window = (short*)window_data;
    float scale = 1.0f/32767.0f;
    int win_len = 800;
	int win_len_half = win_len/2;

    for (i = 0; i < win_len_half; i++)
    {
        fft_buf_tmp[i] = audio_data[i]* scale*fix_window[i];
        fft_buf_tmp[win_len-1-i] = audio_data[win_len-1-i] *scale * fix_window[i];
    }    
	for (i = win_len; i< 1024; i++)
	{	  
			fft_buf_tmp[i] = 0.0f;
	}
	//_riscv_rfft_1024_fast_f32( &S_1024, fft_buf_tmp, result, 0 );
    MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_f32_p(&S,fft_buf_tmp,result,0);

    return RET_SUCCESS;
}


int ci_software_fft_w512( const short *audio_data, float *result )
{
    xSemaphoreTake(fft_xSemaphore,portMAX_DELAY);

    int i = 0;

    
    int win_len = FRAME_SIZE;

    for (i = 0; i < 512; i++)
    {
        fft_buf_tmp[i] = audio_data[i];
    }

    //riscv_rfft_fast_f32( &S, fft_buf_tmp, result, 0 );
    MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_f32_p(&S,fft_buf_tmp,result,0);
    xSemaphoreGive(fft_xSemaphore);

    return RET_SUCCESS;
}

/****************************************************************************
 * read the *.h file please
 ****************************************************************************/
int ci_software_fft_w512_s256( const short *audio_data, const float *window_data, float *result )
{
    xSemaphoreTake(fft_xSemaphore,portMAX_DELAY);

    int i = 0;
    
    int win_len = FRAME_SIZE;

    for (i = 0; i < FRAME_SIZE/2; i++)
    {
    	fft_buf_tmp[i] = audio_data[i] * window_data[i];
    	fft_buf_tmp[win_len-1-i] = audio_data[win_len-1-i] * window_data[i] ;
    }
    
    for (i = win_len; i<  NFFT; i++)
    	fft_buf_tmp[i] = 0.0f;

	//riscv_rfft_fast_f32( &S, fft_buf_tmp, result, 0 );
    MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_f32_p(&S,fft_buf_tmp,result,0);
    xSemaphoreGive(fft_xSemaphore);


    return RET_SUCCESS;
}

/****************************************************************************
 * read the *.h file please
 ****************************************************************************/
int ci_software_ifft_w512_s256( const float *fft, float *result )
{
	xSemaphoreTake(fft_xSemaphore,portMAX_DELAY);
	//riscv_rfft_fast_f32( &S, fft, result, 1 );
    MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_f32_p(&S,(float*)fft,result,1);
	xSemaphoreGive(fft_xSemaphore);
    return RET_SUCCESS;
}


int ci_software_fft_init(riscv_rfft_fast_instance_f32 *S,int len)
{
	//riscv_rfft_fast_init_f32(S,len) ;
	MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_init_f32_p(S,len);
	return RET_SUCCESS;
}

int ci_software_fft(riscv_rfft_fast_instance_f32*S,const float *in, float *result)
{
	//riscv_rfft_fast_f32( S, in, result, 0 );
	MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_f32_p( S, (float*)in, result, 0 );
    return RET_SUCCESS;
}

int ci_software_ifft(riscv_rfft_fast_instance_f32 *S,const float* fft, float* result)
{
    //riscv_rfft_fast_f32( S, fft, result, 1);
	MASK_ROM_LIB_FUNC->fftfunc.riscv_rfft_fast_f32_p( S, (float*)fft, result, 1);
    return RET_SUCCESS;
}

