
#ifndef __CI_BASIC_ALG_FUN_H
#define __CI_BASIC_ALG_FUN_H


//浮点符号注入
static inline float ci_fsgnj_f32(float x,float sign_f)
{
    register float y;
    asm volatile("fsgnj.s %0,%1,%2"
                : "=f"(y)
                : "f"(x),"f"(sign_f));
    return y;
}


//浮点最小值
static inline float ci_fmin_f32(float x1,float x2)
{
    register float min;
    asm volatile("fmin.s %0,%1,%2"
                : "=f"(min)
                : "f"(x1),"f"(x2));
    return min;
}


//浮点最大值
static inline float ci_fmax_f32(float x1,float x2)
{
    register float max;
    asm volatile("fmax.s %0,%1,%2"
                : "=f"(max)
                : "f"(x1),"f"(x2));
    return max;
}


//浮点四舍五入到int32
#define __RV_FLOAT_COV_TO_INT_RNE(f)		\
		({	\
			int32_t result;		\
			float __f = (float)f;	\
			asm volatile ("fcvt.w.s %0,%1,rne" :"=r"(result) :"f"(f)	);	\
			result;	\
		})


static inline float ci_sqrt_f32(float x)
{
    register float y;
    asm volatile("fsqrt.s %0,%1"
                 : "=f"(y)
                 : "f"(x));
    return y;
}

#endif