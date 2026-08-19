<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8E2.0.10%E8%BF%81%E7%A7%BB%E5%88%B02.1.14/ -->

# 从2.0.10迁移到2.1.14

CI130X\_SDK从2.0.10迁移到2.1.14, 先把工程目录从旧SDK中 “CI13XX\_SDK\_ASR\_Offline\_V2.0.10\projects\xxx” 直接拷贝到新的SDK目录 “CI13XX\_SDK\_ASR\_Offline\_V2.1.14\projects\”。然后还需要做下文所描述的修改。

## 脚本修改和同步

* 合成分区bin文件.bat  
  文件路径: “CI130X\_SDK\projects\\*\firmware\合成分区bin文件.bat”  
  同步方式：直接拷贝新版本SDK中示例工程下的文件覆盖旧版本文件。

## 代码修改和同步

* offline\_asr\_pro\_sample工程的main.c  
  文件路径: “CI130X\_SDK\projects\offline\_asr\_pro\_sample\src\main.c”  
  修改方式: 添加代码   
  修改内容：

  ```
  //在UserTaskManageProcess任务创建之后添加以下代码：
  extern void config_adpt_cnt(int enable);
  config_adpt_cnt(ADAPTIVE_CNT_ENABLE);

  extern void config_max_stop_cfd(int enable,int nocnt_max_stop_cfd,int cnt_max_stop_cfd);
  config_max_stop_cfd(MAX_STOP_CFD_ENABLE,MAX_STOP_CFD_NOCNT,MAX_STOP_CFD_CNT);

  extern void config_max_vad_end_frm(int max_vad_end_frm);
  config_max_vad_end_frm(MAX_STOP_VAD_FRM);

  extern int config_base_confidence_count(short base_confidence,unsigned char valid_count);
  config_base_confidence_count(DEFAULT_CONFIDENCE,DEFAULT_CNT);

  extern void config_recover_result(int enable,int mode,int max_frm);
  config_recover_result(RECOVER_RESULT_ENABLE,RECOVER_RESULT_MODE,RECOVER_RESULT_MAX_FRM);

  extern void config_silprob_cnt(float base_silprob,int base_silcnt );
  config_silprob_cnt(DEFAULT_STOP_SILPROB,DEFAULT_STOP_SILCNT);
  ```
* USE\_ALC\_AUTO\_SWITCH\_MODULE   
  文件路径: “CI130X\_SDK\projects\offline\_asr\_pro\_sample\src\user\_config.h”  
  修改方式: 修改宏的定义值  
  修改内容：

  ```
  // 迁移前
  #define USE_ALC_AUTO_SWITCH_MODULE  1
  ```

  ```
  // 迁移后
  #define USE_ALC_AUTO_SWITCH_MODULE  0
  ```

## 新增配置宏

* DEFAULT\_CONFIDENCE   

  ```
  #define DEFAULT_CONFIDENCE      (60) // 配置默认置信度宏 :Excel未配置置信度的词条的默认值, 理论范围[1-255],建议范围[30-150]
  ```
* DEFAULT\_CNT   

  ```
  #define DEFAULT_CNT             (6) // 配置默认cnt宏 :Excel未配置cnt的词条的默认值,理论范围[1-255],建议范围[4-20]
  ```
* ADAPTIVE\_CNT\_ENABLE   

  ```
  #define ADAPTIVE_CNT_ENABLE     (1)  //配置自适应cnt使能宏 ，1：打开自适应 cnt; 0：关闭，加快响应速度，但必须手动在excel配置短词的cnt；
  ```
* MAX\_STOP\_VAD\_FRM

  ```
  #define MAX_STOP_VAD_FRM        (30)  //配置VAD_END帧数宏 最大停顿静音间隔帧数 30=300ms 值越大识别响应时间会加长;过短导致出现截断，长词识别为短词 ,建议：英文/小语种 该值至少35,理论范围 [1-60],建议范围[20-50]
  ```
* MAX\_STOP\_CFD\_ENABLE   

  ```
  #define MAX_STOP_CFD_ENABLE      (0) // 打分超过阈值提前结束功能 0 关闭 1 开启(可提升响应速度)
  ```
* MAX\_STOP\_CFD\_NOCNT

  ```
  #define MAX_STOP_CFD_NOCNT      (50) // MAX_STOP_CFD_ENABLE为1有效 无cnt配置的词，最大结束阈值,理论范围 [5-200],建议范围[50-150]；
  ```
* MAX\_STOP\_CFD\_CNT

  ```
  #define MAX_STOP_CFD_CNT        (60) // MAX_STOP_CFD_ENABLE为1有效 有cnt配置的词，最大结束阈值,至少要比 MAX_STOP_CFD_NOCNT 大,理论范围 [5-200],建议范围[50-150]；
  ```
* DEFAULT\_STOP\_SILPROB

  ```
  #define DEFAULT_STOP_SILPROB    (0.9f) // 识别结束停顿静音概率阈值,理论范围[0-1]，值越大支持的停顿时间越长
  ```
* DEFAULT\_STOP\_SILCNT

  ```
   #define DEFAULT_STOP_SILCNT     (2)  // 识别结束静音连续次数宏：识别结束停顿静音,值越大支持的停顿时间越长,理论范围 [1-200] 建议范围 [1-10]   
   ```
  - RECOVER_RESULT_ENABLE    
  ```c
  #define RECOVER_RESULT_ENABLE    (1)  //语音识别结果回收使能配置（整段语音结束，无有效结果，再回溯搜索打分满足阈值[CNT不满足]的结果）, 开启人声噪音识别率会增加，但集外误识别会提升
  ```
* RECOVER\_RESULT\_MODE

  ```
  #define RECOVER_RESULT_MODE      (0) // RECOVER_RESULT_ENABLE 为1 有效，语音识别结果回收使能作用模式，0 命令词和唤醒词; 1 唤醒词
  ```
* RECOVER\_RESULT\_MAX\_FRM

  ```
  #define RECOVER_RESULT_MAX_FRM   (0) // RECOVER_RESULT_ENABLE 为1 有效，回收使能 在CNT基础上再增加超时帧数 , 10=300 ms ,理论范围 [0-200],建议范围[0-20]
  ```

## 新增接口

* flash\_clk\_div\_init

  ```
  //用于初始flash控制器的预分频系数。
  int32_t flash_clk_div_init(spic_base_t spic);
  ```