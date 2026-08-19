<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E8%87%AA%E5%8A%A8%E5%A2%9E%E7%9B%8A%E6%8E%A7%E5%88%B6%E7%AE%97%E6%B3%95/ -->

# 自动增益控制算法

**1.算法功能配置步骤如下(任意单麦算法或单麦算法组合下面配置都有效)：**

打开CI13XX\_SDK\_ALG\_PRO\_Vx.x.x\project\_file\makefile文件，修改USE\_ALC\_AUTO\_SWITCH\_MODULE:= 1，如下图：
![CI_ALG_TYPE配置](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-ALG/CI13XX_SDK_ASR_ALG_V2.6.3/API%E6%8C%87%E5%8D%97/%E7%AE%97%E6%B3%95%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/alc%E4%BD%BF%E8%83%BD.png)

**2. 当前版本自动增益控制算法仅单mic可用，只对68dB以上的稳态噪声有效**