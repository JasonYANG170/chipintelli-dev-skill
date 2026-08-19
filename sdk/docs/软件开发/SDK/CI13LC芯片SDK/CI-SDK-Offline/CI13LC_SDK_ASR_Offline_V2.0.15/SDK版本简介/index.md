<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/SDK%E7%89%88%E6%9C%AC%E7%AE%80%E4%BB%8B/ -->

# 离线语音识别SDK版本介绍

---

## **CI13LC\_SDK\_ASR\_Offline\_V2.0.15**

版本 2.0.15 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8EV1.2.1%E8%BF%81%E7%A7%BB%E5%88%B0V2.0.15/)

本次更新：

1. 内存、算力、识别效果优化；
2. 优化自学习功能中的部分问题；
3. 增加1316XP，13082等板级配置；
4. 增加多意图功能；

* 工具更新：

1. 增加多芯片型号的选择支持；
2. 工具栏增加菜单栏，增加擦除flash，读校验码，显示固件信息等功能；
3. 升级界面优化；

---

## **CI13LC\_SDK\_ASR\_Offline\_V1.2.1**

版本 1.2.1 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8EV1.1.4%E8%BF%81%E7%A7%BB%E5%88%B0V1.2.1/)

本次更新：

1. asr 优化和优化，节省sram；
2. 自学习可以学习已经是命令词的词条；
3. 播放完后识别词条的速度加快；
4. 工具更新：支持读取校验码；

---

## **CI13LC\_SDK\_ASR\_Offline\_V1.1.4**

版本 1.1.4 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8EV1.0.8%E8%BF%81%E7%A7%BB%E5%88%B0V1.1.4/)

本次更新：

1. 识别优化，模型适配pro4；
2. 支持命令词自学习功能；
3. 增加13242及更多型号的板级配置；
4. 改对pro4模型的支持；
5. 增加获取帧能量的结果；
6. ！！！！！特别注意：自适应cnt宏定义默认开启：ADAPTIVE\_CNT\_ENABLE，如需要提升识别相应速度，需要该宏定义设置0和注意调节cmd\_info 中的特殊词计数数值；

* 打包工具升级为V3.9.7：

1. 提升兼容性；

---

## **CI13LC\_SDK\_ASR\_Offline\_V1.0.8**

版本 1.0.8 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8EV1.0.7%E8%BF%81%E7%A7%BB%E5%88%B0V1.0.8/)

本次更新：

1、修正：串口收到指令被动组合播放，而且组合播放的声音<50ms时，快速发播放指令有概率概率出现死机的情况；

2、修改对pro4模型的支持；

3、增加支持linux 库；

---

## **CI13LC\_SDK\_ASR\_Offline\_V1.0.7\_Alpha**

版本 1.0.7

本次更新：

1、支持芯片CI13161/CI13162/CI13241/CI13242；

2、支持pro，pro2，pro3，pro4的多种pro模型，详情查看sdk中的说明文档；