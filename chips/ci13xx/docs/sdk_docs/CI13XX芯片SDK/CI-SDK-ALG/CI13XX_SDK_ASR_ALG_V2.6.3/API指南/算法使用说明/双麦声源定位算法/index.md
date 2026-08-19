<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8F%8C%E9%BA%A6%E5%A3%B0%E6%BA%90%E5%AE%9A%E4%BD%8D%E7%AE%97%E6%B3%95/ -->

# 双麦声源定位算法

该算法能进行声源方位角度估计，当前版本支持0-180度检测范围，分辨率为10°，双mic位置需处于同一平面相同朝向，推荐麦间距为4cm~6cm(超过约定范围的mic间距需要重新训练模型支持)，该算法仅双mic可用。

**1.算法功能配置步骤如下：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，对CI\_ALG\_TYPE修改为CI\_ALG\_TYPE := $(USE\_AI\_DOA)

**CI\_ALG\_TYPE变量和算法功能对应说明请参考：[算法功能使用说明](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/#3)**

**2. 该算法参数宏说明在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_main\user\_config.h文件中**

```
 //doa输出类型：1-唤醒词输出角度  2-命令词输出角度 3-唤醒次和命令词都输出角度
#define AI_DOA_OUT_TYPE                 3
```

**3. 该算法检测到角度信息回调函数在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_doa\doa\_app\_handle.c文件中**

```
/**
 * @brief 角度信息输出-检测到声源角度信息后，系统会自动回调该函数，用户可以通过该函数的参数doa_angle拿到当前声源角度值，参数audio_state为预留参数，用户不用关注
 */
void ci_doa_get_cb(int audio_state, int doa_angle)
```

**4. 声源定位算法请把external\model\doa(声源定位)中[60004]nn\_dual\_mic\_doa\_vxxxx.bin算法模型，复制到projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\firmware\dnn文件夹中**

**5. DOA算法参数配置**
如下算法参数用户只需关注distance，其他参数算法内部使用，用户无需关注

```
const doa_config_t doa_config =
{
    .distance = 40,        //双mic间距(单位mm),40表示当前双mic间距为4cm      
};
```

**调节参数说明**
distance: (调节mic间距，范围4~6cm)

注意

1. 双mic位置需处于同一平面相同朝向，推荐麦间距为4cm~6cm。
2. 声源定位算法支持多种不同方式输出角度信息，可通过AI\_DOA\_OUT\_TYPE宏来设置。
3. 声源定位需搭配该算法的前端算法模型使用。