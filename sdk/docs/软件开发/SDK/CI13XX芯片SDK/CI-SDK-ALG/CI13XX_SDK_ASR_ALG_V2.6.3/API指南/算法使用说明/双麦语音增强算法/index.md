<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E8%AF%AD%E9%9F%B3%E5%A2%9E%E5%BC%BA%E7%AE%97%E6%B3%95/ -->

# 双麦语音增强使用说明

该算法主要用于半稳态噪声，如新闻噪声、音乐噪声环境下，用于抑制噪声和干扰，提升目标语音信噪比。算法只支持双麦阵列，要求双麦克风位置需处于同一平面且朝向相同，推荐麦间距为4厘米。

**1.算法功能配置步骤如下：**

打开CI-SDK-ASR-ALG\_Vx.x.x\projects\offline\_asr\_alg\_pro\_sample\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_BF)

**CI\_ALG\_TYPE变量配置请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 该算法参数宏说明在CI-SDK-ASR-ALG\_Vx.x.x\projects\offline\_asr\_alg\_pro\_sample\app\app\_main\user\_config.h文件中**

```
//1:全深度分离更耗内存(单双网络都可以用) 0：半深度分离(唤醒词做深度分离，命令词不做，只能用双网络)
#define BF_DEEPSE_MODE                  1 
//1:开启ASR打分是否有效判断功能 0：关闭ASR打分是否有效判断功能 该功能只针对全深度分离和半深度分离  
#define BF_ASR_VALID_MODE               0
```

注意

1. 噪声或者目标人声有一个声源距离双麦阵列1米内效果较好；
2. 使用过程中无需设置初始目标声源角度和麦克风间距，但推荐麦间距为4厘米。使用过程中请尽量避免噪声和目标人声同方向，此情况下算法性能会降低。噪声与人声夹角越大，降噪效果越好；
3. 开启双麦语音增强算法后，sdk默认关闭alc，左右麦克风设置ADC固定增益设置为20；