<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B9%E6%A3%80%E6%B5%8B%E7%AE%97%E6%B3%95/ -->

# 男女声纹检测算法

男女声纹检测算法基于声纹特征检测目标人声是男声还是女声

**1.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_WMAN\_VPR)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 该算法参数宏说明在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_main\user\_config.h文件中，可调整的参数如下(如无特殊需求，建议都使用sdk中的默认宏配置)：**

```
//VP_USE_FRM_LEN：声纹计算的窗长，单位为ms，建议范围1200-1500，值越大消耗内存越多（每增加100，内存增加8KB）
#define VP_USE_FRM_LEN                  1200   
//WMAN_PLAY_EN：是否开启男女声纹识别播报，默认开启                         
#define WMAN_PLAY_EN                    1
```

**3. 男女声纹检测算法请把external\model\wman\_vpr(男女声纹)中[60008]VGR\_model\_xxxx\_vx算法模型，复制到projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\firmware\dnn文件夹中**

注意

1. 男女声纹检测需搭配该算法的前端算法模型使用。

**4. 男女声纹识别结果在vpr\_run\_one\_recognition函数中，该函数位于CI-SDK-ASR-ALG\_Vx.x.x\projects\components\VPR\voice\_print\_recognition.c中，如下图:**

![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B91.png)
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E7%94%B7%E5%A5%B3%E5%A3%B0%E7%BA%B92.png)

当前SDK示例会打印对应的结果和播报对应的播报音，用户可在识别结果处添加对应的应用逻辑。