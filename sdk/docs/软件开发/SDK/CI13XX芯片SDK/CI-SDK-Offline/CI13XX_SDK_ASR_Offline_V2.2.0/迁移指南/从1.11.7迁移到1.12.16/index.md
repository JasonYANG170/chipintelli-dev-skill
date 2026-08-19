<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8E1.11.7%E8%BF%81%E7%A7%BB%E5%88%B01.12.16/ -->

# 从1.11.7迁移到1.12.16

CI13XX\_SDK\_ASR\_Offline从1.11.7迁移到1.12.16, 先把工程目录从旧SDK中 “CI13XX\_SDK\_ASR\_Offline\_V1.11.7\projects\xxx” 直接拷贝到新的SDK目录 “CI13XX\_SDK\_ASR\_Offline\_V1.12.16\projects\”。然后还需要做下文所描述的修改。

## 板级配置修改：

* 在板级配置文件中的串口引脚配置函数中，串口的引脚使用开漏模式时，需要增加延时
  文件路径：自定义的板级配置文件

```
void pad_config_for_uart(UART_TypeDef *UARTx)
{
    if (UARTx == UART0)
    {
        dpmu_set_io_reuse(PB5,SECOND_FUNCTION);
        dpmu_set_io_reuse(PB6,SECOND_FUNCTION);
        #if UART0_PAD_OPENDRAIN_MODE_EN
        dpmu_set_io_pull(PB5,DPMU_IO_PULL_DISABLE);  
        dpmu_set_io_pull(PB6,DPMU_IO_PULL_DISABLE);  
        dpmu_set_io_open_drain(PB5, ENABLE);
        _delay_10us_240M(100);  //需增加此延时, 等待开漏模式生效
        #else
        dpmu_set_io_pull(PB6,DPMU_IO_PULL_UP);  
        #endif
    }
    else if (UARTx == UART1)
    {
        dpmu_set_io_reuse(PA2,FORTH_FUNCTION);
        dpmu_set_io_reuse(PA3,FORTH_FUNCTION);
        #if UART1_PAD_OPENDRAIN_MODE_EN
        dpmu_set_io_pull(PA2,DPMU_IO_PULL_DISABLE);  
        dpmu_set_io_pull(PA3,DPMU_IO_PULL_DISABLE);  
        dpmu_set_io_open_drain(PA2, ENABLE);
        _delay_10us_240M(100);  //需增加此延时, 等待开漏模式生效
        #else
        dpmu_set_io_pull(PA3,DPMU_IO_PULL_UP);  
        #endif
    }
    else if (UARTx == UART2)
    {
        dpmu_set_io_reuse(PA5,FORTH_FUNCTION);
        dpmu_set_io_reuse(PA6,FORTH_FUNCTION);
        #if UART2_PAD_OPENDRAIN_MODE_EN
        dpmu_set_io_pull(PA5,DPMU_IO_PULL_DISABLE);  
        dpmu_set_io_pull(PA6,DPMU_IO_PULL_DISABLE);  
        dpmu_set_io_open_drain(PA5, ENABLE);
        _delay_10us_240M(100);  //需增加此延时, 等待开漏模式生效
        #else
        dpmu_set_io_pull(PA6,DPMU_IO_PULL_UP);  
        #endif
    }
}
```

* 开启AEC功能时，需要修改以下宏定义：
  文件路径：CI130X\_SDK\projects\\*\src\user\_config.h

```
// 迁移前:
#if USE_AEC_MODULE
#define HOST_CODEC_CHA_NUM           2
#endif
```

```
// 迁移后:
#if USE_AEC_MODULE
#define PAUSE_VOICE_IN_WITH_PLAYING  0
#define IF_JUST_CLOSE_HPOUT_WHILE_NO_PLAY   1
#define HOST_CODEC_CHA_NUM           2
#endif
```

* 添加波特率同步短间隔时间，用于解决一次波特率同步失败后，后续波特率同步间隔过长的现象。
  文件路径：CI130X\_SDK\projects\\*\src\user\_config.h

```
#define BAUDRATE_FAST_SYNC_PERIOD       5000         // 一次波特率校准识别后，下一次同步周期，单位毫秒
```

## API修改：

* dsu\_init

```
//迁移前：   
void dsu_init(void);
```

```
//迁移后：   
void dsu_init(unsigned int addr);
```

在main.c中调用时，需要传入dsu的地址，例如：

```
extern char SRAM_HOST_END_ADDR;
dsu_init((uint32_t)&SRAM_HOST_END_ADDR);
```

* copyright\_verification1

  ```
  //迁移前：
  /**
   * @brief 通过自定义加密算法校验软件是否被拷贝
   * 
   * @param ec_func 用于传入自定义加密算法的函数指针
   * @param out_len 用于指定加密后的密文的长度
   * @return int 返回校验函数是否正常运行，0：未正常运行或校验未通过，1：校验通过。
   */
  extern int copyright_verification1(ENCRYPT_CALLBACK ec_func, int out_len);
  ```

  ```
  //迁移后：
  /**
   * @brief 通过自定义加密算法校验软件是否被拷贝
   * 
   * @param ec_func 用于传入自定义加密算法的函数指针
   * @param out_len 用于指定加密后的密文的长度
   * @param firm_ver 固件版本
   * @return int 返回校验函数是否正常运行，0：未正常运行或校验未通过，1：校验通过。
   */
  extern int copyright_verification1(ENCRYPT_CALLBACK ec_func, int out_len, uint8_t firm_ver);
  ```
* copyright\_verification2

  ```
  //迁移前：
  /**
   * @brief 通过默认加密算法和密码校验软件是否被拷贝
   * 
   * @param password 加密密码
   * @param password_len 加密密码的长度
   * @return int 返回校验函数是否正常运行，0：未正常运行或校验未通过，1：校验通过。
   */
  extern int copyright_verification2(char * password, int password_len);
  ```

  ```
  //迁移后：
  /**
   * @brief 通过默认加密算法和密码校验软件是否被拷贝
   * 
   * @param password 加密密码
   * @param password_len 加密密码的长度
   * @param firm_ver 固件版本
   * @return int 返回校验函数是否正常运行，0：未正常运行或校验未通过，1：校验通过。
   */
  extern int copyright_verification2(char * password, int password_len, uint8_t firm_ver);
  ```
* pause\_asr

  ```
  //迁移前：
  void pause_asr(void);
  ```

  ```
  //迁移后：
  //参数voice_in_mute：用于设置在暂停唤醒词识别时，是否同时将录音入的音频静音。
  void pause_asr(uint8_t voice_in_mute);
  ```
* resume\_asr

  ```
  //迁移前：
  void resume_asr(void);
  ```

  ```
  //迁移后：
  //参数voice_in_mute：用于传递录音的静音状态。
  void resume_asr(uint8_t voice_in_mute);
  ```

## 代码逻辑修改：

* 用到自学习功能的工程，同步自学习模块中对AEC的处理逻辑:文件路径：CI130X\_SDK\projects\cwsl\_sample\src\system\_msg\_deal.c同步内容：同步宏USE\_AEC\_MODULE管控的代码。
* 上电后的监听状态切换放到上电播报之前：
  文件路径：CI130X\_SDK\projects\cwsl\_sample\src\system\_msg\_deal.c

```
//迁移前：
pause_voice_in();

                        #if PLAY_WELCOME_EN
                        vTaskDelay(pdMS_TO_TICKS(300));             // 等待功放开启                      
                        prompt_play_by_cmd_string("<welcome>", -1, play_exit_wakeup_done_cb,true);
                        #else
                        play_exit_wakeup_done_cb(INVALID_HANDLE);
                        #endif
```

```
//迁移后：
                        pause_voice_in();

                        #if PLAY_WELCOME_EN
                        vTaskDelay(pdMS_TO_TICKS(300));             // 等待功放开启
                        play_exit_wakeup_done_cb(INVALID_HANDLE);   // 上电播报之前先切换到监听唤醒词状态
                        prompt_play_by_cmd_string("<welcome>", -1, NULL,true);
                        #else
                        play_exit_wakeup_done_cb(INVALID_HANDLE);
                        #endif
```

## 脚本修改：

* 合成分区bin文件.bat
  路径：CI130X\_SDK\projects\\*\firmware\合成分区bin文件.bat
  更新原因：更换部分操作系统不兼容的命令。

## 新增配置宏：

* BAUDRATE\_FAST\_SYNC\_PERIOD

  ```
  #define BAUDRATE_FAST_SYNC_PERIOD   5000         // 一次波特率校准识别后，下一次同步周期，单位毫秒
  ```
* ONE\_SHOT\_ENABLE

  ```
  #define ONE_SHOT_ENABLE                     0              //1：开启oneshot功能。0：关闭oneshot功能
  ```