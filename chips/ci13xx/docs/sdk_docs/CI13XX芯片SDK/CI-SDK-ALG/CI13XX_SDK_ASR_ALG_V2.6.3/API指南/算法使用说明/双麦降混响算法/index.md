<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E9%99%8D%E6%B7%B7%E5%93%8D%E7%AE%97%E6%B3%95/ -->

# 双麦降混响算法

该算法在高混响的环境下能消除混响提升识别效果，该算法仅双mic可用。

**1.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_DEREVERB)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 该算法参数宏说明在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_main\user\_config.h文件中**

```
//默认0:算法起效频率160HZ-4800HZ 消耗28KB内存  1: 算法起效频率0-8000HZ 消耗49KB内存
#define DEREVERB_FREQ_RANGE_INDEX       0
```

**3. 降混响算法参数配置**

```
const dereverb_config_t dereverb_config =
{
#if DEREVERB_FREQ_RANGE_INDEX
    .startHz = 0.0f,
    .endHz = 8000.0f
#else
    .startHz = 160.0f,    //算法起效的起始频率
    .endHz = 4800.0f      //算法起效的结束频率
#endif
};
调节参数说明，请根据应用场景需求和当前剩余内存值进行调节：
- startHZ:算法起效的起始频率  
- endHZ:算法起效的结束频率  
- 范围:0-8KHZ,调大会增加一定的算法力和内存消耗
- 内存消耗：160HZ-4800HZ 消耗28KB内存  0-8000HZ 消耗49KB内存
```