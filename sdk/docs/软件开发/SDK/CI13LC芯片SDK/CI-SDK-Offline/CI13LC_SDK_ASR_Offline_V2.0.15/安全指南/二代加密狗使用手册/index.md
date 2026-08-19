<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C/ -->

# 启英泰伦二代加密狗使用手册

加密狗功能说明与整体工作流程如下图1所示：

![929cab1ee03ce6cd4d6d57ad54a5ee4](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-2.png)

图1

加密狗外观如图2所示

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-3.png)

图2

## 1 工厂操作流程

### 1.1上位机使用流程

1. 双击打开工具ci\_encryption\_tool\_工厂\ci\_encryption\_tool\_工厂.exe，如图2所示是程序运行成功之后，检测到加密狗硬件并将硬件设备列表显示出来的。
2. 加密狗插入电脑USB。
3. 选中加密狗->设置输出路径->点击生成密钥(可生成多次直至写入加密狗)，显示”成功”后，将在输出路径中生成加密狗对应.key密钥文件。![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-4.png)

   图3
4. 将步骤3中标号2内路径下的dongle\_num\_xx\_key.key（xx是工具右侧的设备列表中被选中的加密狗的设备号）发给方案商。
5. 方案商会根据.key秘钥与固件生成提供，如图3所示的名为dongle\_xx\_suth\_file(xx同样的表示设备号)文件夹。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-5.png)

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-6.png)

   图4
6. 按图4所示中红色标记的步骤，选择 **加密狗设备->设置路径->写入加密狗** 将授权文件写入加密狗，等待操作完成即可。

   ![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-7.png)

   图5
7. 可通过点击图4”获取烧录次数”来查看加密狗内剩余可烧录次数。

### 1.2 加密固件烧录

将加密后的固件dongle\_xx\_encrypt\_firmware.bin放到离线烧录器的TF卡中，并按照图5的配置方式修改batch\_production\_config.ini文件。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-8.png)

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-9.png)

图6

按图6所示，将加密狗插入到离线烧录器，开机后按照普通烧录器使用方式进行烧录。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-10.png)

图7

## 2 方案商操作流程

### 2.1. 双击打开工具 ci\_encryption\_tool\_方案商\ci\_encryption\_tool\_方案商.exe。

### 2.2. 按照图5红色的标号所示的步骤，依次填入：

a. 厂商ID（可自定义）。

b. 点击导入固件，在对话框中选择需要加密的固件。

c. 点击单次导入密钥文件，选择从工厂获得的xxx.key文件。

d. 填入固件的密码，这里的固件密码是方案商在制作固件的时候固化在sdk中的密码。

e. 烧录次数可以根据需要让代工厂烧录多少次来限制次数。

f. 点击设置输出路径，选择授权文件夹存放路径。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-11.png)

图8

### 2.3. 得到授权文件夹之后，将授权文件夹传给工厂即可。

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-12.png)

![](https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI13LC%E8%8A%AF%E7%89%87SDK/CI-SDK-Offline/CI13LC_SDK_ASR_Offline_V2.0.15/%E5%AE%89%E5%85%A8%E6%8C%87%E5%8D%97/img/%E4%BA%8C%E4%BB%A3%E5%8A%A0%E5%AF%86%E7%8B%97%E4%BD%BF%E7%94%A8%E6%89%8B%E5%86%8C-13.png)

图9