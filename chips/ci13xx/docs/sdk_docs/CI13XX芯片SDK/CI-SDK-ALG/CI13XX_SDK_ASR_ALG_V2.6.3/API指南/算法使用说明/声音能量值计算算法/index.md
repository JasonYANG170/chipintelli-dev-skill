<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%A3%B0%E9%9F%B3%E8%83%BD%E9%87%8F%E5%80%BC%E8%AE%A1%E7%AE%97%E7%AE%97%E6%B3%95/ -->

# 声音能量值计算算法

声音能量值计算算法用于输出唤醒词或者命令词的音频能量值。

**1.算法功能配置步骤如下(开启任一算法或算法组合下面配置都有效)：**

1. 使用音频能量输出功能，需打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，修改USE\_PWK\_ENABLE:= 1；如下图：

![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E8%83%BD%E9%87%8F%E8%AE%A1%E7%AE%97.png)

**2.能量值计算结果输出**
1. 计算出能量值会回调ci\_pwk\_get\_cb函数，在projects\CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\app\app\_main\system\_msg\_deal.c文件中，用户可以从回调函数的参数db\_val拿到当前计算的能量值

```
/**
 * @brief 音频能量值信息输出，db_val为当前计算的能量值
 */
void ci_pwk_get_cb(int db_val)
{
    mprintf("--------pwk db val: %d\n", db_val);
}
```