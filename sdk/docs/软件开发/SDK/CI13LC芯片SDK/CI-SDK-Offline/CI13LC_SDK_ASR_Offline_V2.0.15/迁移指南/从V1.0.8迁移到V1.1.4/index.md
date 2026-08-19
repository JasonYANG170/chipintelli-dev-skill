<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8EV1.0.8%E8%BF%81%E7%A7%BB%E5%88%B0V1.1.4/ -->

# 从V1.0.8迁移到V1.1.4

本次更新：

识别优化，模型适配pro4；

支持命令词自学习功能；

增加13242及更多型号的板级配置；

改对pro4模型的支持；

增加获取帧能量的结果；

！！！！！特别注意：自适应cnt宏定义默认开启：ADAPTIVE\_CNT\_ENABLE，如需要提升识别相应速度，需要该宏定义设置0和注意调节cmd\_info 中的特殊词计数数值；

打包工具升级为V3.9.7：
提升兼容性；