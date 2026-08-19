<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/SDK%E7%89%88%E6%9C%AC%E7%AE%80%E4%BB%8B/ -->

# 离线语音识别SDK版本介绍

---

## **CI13XX\_SDK\_ASR\_Offline\_V2.2.0**

版本 2.2.0 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8E2.1.14%E8%BF%81%E7%A7%BB%E5%88%B02.2.0/)

本次更新：

1. 支持DNN模型的切换，如支持中英文时，支持不同版本的DNN pro5模型切换，不支持pro5切换pro；
2. 增加flash 保护功能；
3. 删除一些离线版本不再支持的算法宏定义 更新工具为 V3.9.9 1,可以查询固件的校验码和查看分区地址。

---

## **CI13XX\_SDK\_ASR\_Offline\_V2.1.14**

版本 2.1.14 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8E2.0.10%E8%BF%81%E7%A7%BB%E5%88%B02.1.14/)

本次更新：

1. 支持各种pro模型，优化适配性及提升识别稳定性；
2. 修正特定条件下播放音组合不完整的情况；
3. 合成播放音默认使用-b16，减小voice的空间；
4. 识别速度相关：ADAPTIVE\_CNT\_ENABLE 1：自适应cnt， 0：可以获取更快的相应速度，但是必须要修改cmd\_info中词条的合理cnt数值； 5、打包工具更新为 V3.9.7。

---

## **CI13XX\_SDK\_ASR\_Offline\_V2.0.10**

版本 2.0.10 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8E1.12.16%E8%BF%81%E7%A7%BB%E5%88%B02.0.10/)

本次更新：

1. 自然说初版SDK；
2. user\_config.h 优化， 更适合新手使用；
3. 该SDK可用于 离线自然说方案开发，也可以用于的命令词的方案开发；
4. pro 模型识别更精准，抗噪性能也更好，选择模型时请优先选择最高版本号；
5. 该SDK只保留了工程 offline\_asr\_pro\_sample；其他的sample（如命令词自学习）后续版本添加；
6. 更新打包工具。

---

## **CI13XX\_SDK\_ASR\_Offline\_V1.12.16**

版本 1.12.16 ☞[迁移指南](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13XX%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13XX_SDK_ASR_Offline_V2.2.0/%E8%BF%81%E7%A7%BB%E6%8C%87%E5%8D%97/%E4%BB%8E1.11.7%E8%BF%81%E7%A7%BB%E5%88%B01.12.16/)

本次更新：

1. 识别性能稳定性提升，避免极小概率某个词识别不好的情况；
2. 波特率更广泛的适配性；
3. 优化自学习和AEC 同时存在的代码逻辑 重要提示： 基于固定命令词的sdk 的最后版本，后续不再添加新的属性功能变化。

---

## **CI13XX\_SDK\_ASR\_Offline\_V1.11.7**

版本 1.11.7

本次更新：

1. 更新识别库，优化内存；
2. 修改音量调为0的时候，将DAC的数字增益也调为0，避免还能听到细微的声音；
3. 解决特定条件下，复位UART数据未发完的问题；
4. 打包升级工具更新为 V3.8.3；
5. 打包升级工具解决打包完固件后直接跳转升级界面时, 固件信息没有及时更新显示的问题；
6. 打包升级工具打包固件版本号增加点符号, 仅添加到文件名, 不写入分区表。