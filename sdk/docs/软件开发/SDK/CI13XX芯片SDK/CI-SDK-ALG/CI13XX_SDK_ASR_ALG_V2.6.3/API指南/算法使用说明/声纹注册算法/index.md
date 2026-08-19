<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%A3%B0%E7%BA%B9%E6%B3%A8%E5%86%8C%E7%AE%97%E6%B3%95/ -->

# 声纹注册算法

声纹注册算法当前推荐最多注册4个人，人数越多会影响注册效果，如需注册更多人数，需确认效果达到使用要求；同时注册过的声纹支持单个删除或全部删除。

**1.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，将CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_VPR)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 该算法参数宏说明在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_main\user\_config.h文件中, 可调整的参数如下(如无特殊需求，建议都使用sdk中的默认宏配置)：**

```
//声纹计算的窗长，单位为ms, 建议范围1200-1500，值越大消耗内存越多（每增加100，内存增加8KB）
#define VP_USE_FRM_LEN                  1200      
//声纹阈值-建议范围(0.48-0.68)，值越大，灵敏度越低，误识越低，识别率下降，需要更严格的匹配注册的模版
#define VP_THR_FOR_MATCH                (0.52f)  
//声纹注册时重复录入次数 -注册时的次数 
#define VP_REC_TIMES                    3         
//声纹识别功能允许的最大模版(用户)数,最大4个 重要说明：每个模版单次约占0.8KB NV空间，三次2.4KB
#define MAX_VP_TEMPLATE_NUM             3         
//注册声纹时最大超时等待时间（秒)
#define MAX_VP_REG_TIME                 10
```

**3. 声纹注册算法请把external\model\vpr(声纹注册)中[60001]VPR\_model\_v00xx.bin算法模型，复制到projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\firmware\dnn文件夹中**

注意

1. 固件打包时，如果NV data分区空间过小，影响模板数据存储，导致无法正常识别已注册的声纹。
2. 声纹注册需搭配该算法的前端算法模型使用。

**4. 声纹注册结果说明：**

声纹注册和注册以后的识别结果在vpr\_callback回调函数中，该函数位于CI-SDK-ASR-ALG\_Vx.x.x\projects\components\VPR\voice\_print\_recognition.c中，如下图:

![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/vpr_registe.png)

**vpr\_callback函数参数rst:表示返回当前注册状态，reg\_index:表示当前注册到第几个模版**

rst注册状态有如下几类：

```
    typedef enum
    {
        vpr_reg_successed,          // 注册成功
        vpr_reg_failed,             // 注册失败
        vpr_rec_successed,          // 识别成功
        vpr_rec_failed,             // 识别失败
        vpr_reg_resample,           //继续重复录入声纹
        vpr_reg_resample_failed     //重复录入失败(注册一个模板可能需要录入多次)。
    }vpr_callback_rst_t;
```

用户可以根据rst状态在对应的case添加后续对应需要执行的逻辑代码，示例中只对当前注册和识别状态做了对应的结果进行了播报。