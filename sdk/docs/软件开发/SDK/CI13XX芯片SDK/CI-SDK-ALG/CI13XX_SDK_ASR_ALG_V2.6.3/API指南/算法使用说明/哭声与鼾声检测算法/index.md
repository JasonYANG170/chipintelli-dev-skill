<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%93%AD%E5%A3%B0%E4%B8%8E%E9%BC%BE%E5%A3%B0%E6%A3%80%E6%B5%8B%E7%AE%97%E6%B3%95/ -->

# 哭声与鼾声检测算法

该算法能检测环境中出现的目标声音事件，受内存限制，暂不支持语音识别功能。

**1.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，将CI\_ALG\_TYPE进行修改，哭声算法修改成CI\_ALG\_TYPE := $(USE\_SED\_CRY)，鼾声算法修改成CI\_ALG\_TYPE := $(USE\_SED\_SNORE)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 该算法参数宏说明在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_main\user\_config.h文件中**

**哭声检测参数调节:**

```
//可根据具体需求修改,范围为(0~1)float类型-建议范围(0.5-0.6f),值越大，灵敏度越低
#define THRESHOLD_CRY                   0.53f  
//可根据具体需求修改,最大5次(算法计算几次给结果)   
#define TIMES_CRY                       3
```

**鼾声检测参数调节:**

```
//可根据具体需求修改,范围为(0~1)float类型-建议范围(0.5-0.6f),值越大，灵敏度越低
#define THRESHOLD_SNORE                   0.53f     
//可根据具体需求修改,最大5次(算法计算几次给结果)
#define TIMES_SNORE                       3
```

**3. 哭声鼾声检测结果回调函数在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_sed\sed\_app\_host.c文件中**

当检测到哭声或鼾声都会回调sed\_rslt\_cb()函数，用户可以在该函数中检测到以后的应用逻辑，示例中检测到以后只进行播报音播放，如下图：

![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%93%AD%E5%A3%B0%E5%92%8C%E9%BC%BE%E5%A3%B0%E6%A3%80%E6%B5%8B.png)

**4. 哭声检测算法请把external\model\sed\_cry(哭声检测)中[60002]sed\_cryx\_mxx.bin算法模型，鼾声检测算法请把external\model\sed\_snore(鼾声检测)中[60005]sed\_snorex\_mxx.bin算法模型，复制到projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\firmware\dnn文件夹中**

注意

1. 哭声鼾声检测算法不支持语音识别，语音识别声学模型可用[0]reserve.bin代替。
2. 哭声鼾声检测需搭配该算法对应的神经网络模型使用。