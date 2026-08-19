<!-- Source: https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/ -->

# 单端和差分麦克风的使用说明

## 一、介绍

### 1.1 背景与目标

启英泰伦语音芯片使用差分麦克风搭配差分电路设计，单端麦克风搭配单端电路设计，在实际应用中会出现错误搭配的情况，现通过此文档对这两种电路设计进行说明。

## 二、麦克风电路硬件设计

### 2.1 CI-130X系列芯片麦克风差分电路设计

![](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-1.png)

### 2.2 CI-130X系列芯片麦克风单端电路设计

![](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-2.png)

### 2.3 CI-13LC系列芯片麦克风差分电路设计

![](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-3.png)

### 2.4 CI-13LC系列芯片麦克风单端电路设计

![](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-4.png)

## 三、单端麦克风和差分麦克风的使用说明

### 3.1 信号传输方式​

![](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-5.png)

图-1

单端麦克风使用单根信号线传输音频信号，但外部包裹金属屏蔽层​（如铜网或铝箔），接地后用于阻挡外部电磁干扰（EMI）。屏蔽网通常与信号地线相连，形成单端接地结构，如图-1所示。​

![](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E4%BA%A7%E5%93%81%E7%BB%93%E6%9E%84%E8%AE%BE%E8%AE%A1/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/img/%E5%8D%95%E7%AB%AF%E9%BA%A6%E5%85%8B%E9%A3%8E%E5%92%8C%E5%B7%AE%E5%88%86%E9%BA%A6%E5%85%8B%E9%A3%8E%E7%9A%84%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E-6.png)

图-2

差分麦克风通过正极（+）和负极（-）两根信号线传输相位相反的信号，无需依赖屏蔽网抗干扰，依赖双绞线的差分信号抵消共模噪声，如图-2所示。

### 3.2 抗干扰能力对比

---

| 特性 | 单端麦克风 | 差分麦克风 |
| --- | --- | --- |
| 抗电磁干扰（EMI） | 依赖屏蔽网阻挡外部干扰 | 通过差分信号抵消干扰，不依赖屏蔽网 |
| 抗共模噪声 | 较弱（噪声直接叠加到单端信号） | 极强（共模噪声被差分信号抑制） |
| 地线噪声抑制 | 屏蔽网可减少地环路干扰，但无法完全消除 | 完全不受地线噪声影响（双信号线浮动参考） |

---

### 3.3 与硬件电路错误搭配的影响

### 3.3.1 单端电路设计使用了差分麦克风

这种情况一般影响不大，相同环境识别率与正确搭配相比相差不大。

### 3.3.2 差分电路设计使用了单端麦克风

这种情况有可能会对识别率产生影响，因为屏蔽网处的电磁干扰将通过麦克风负极直接进入芯片引脚。

## 四、麦克风物料选配

用于我司语音芯片的麦克风参数规格有两点需要注意的，灵敏度必须为-32±3dB，信噪比必须为≥65dB。推荐用户直接采用我司验证后的麦克风设计方案，可参考我司[外围器件兼容列表](https://document.chipintelli.com/%E7%A1%AC%E4%BB%B6%E5%BC%80%E5%8F%91/%E5%A4%96%E5%9B%B4%E5%99%A8%E4%BB%B6%E5%85%BC%E5%AE%B9%E5%88%97%E8%A1%A8/)选择合适的麦克风。