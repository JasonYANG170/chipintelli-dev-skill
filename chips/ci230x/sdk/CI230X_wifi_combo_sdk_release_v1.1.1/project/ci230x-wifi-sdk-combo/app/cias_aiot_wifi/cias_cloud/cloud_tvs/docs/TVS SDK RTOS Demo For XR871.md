## TVS SDK RTOS Demo For XR871

#### 1、SDK的部署流程

- 从github上下载XR871的SDK：

https://github.com/XradioTech/XR871SDK.git

- 进入XR871SDK\project目录；

- 将tvs sdk压缩包中的tvs_sdk目录和tvs_demo目录拷贝到project目录中；

- 开启cygwin/Msys/ubuntu控制台，在XR871SDK\project\tvs_demo\gcc下执行：

  make build

- 编译结束生成img文件，通过PhoenixMC工具烧录到开发板中即可

#### 2、配置WIFI

在XR871SDK\project\tvs_demo\main.c中配置WIFI SSID和PSK：

~~~c
void config_wifi_info() {
	show_text_in_ui("wifi starting");
	OS_MSleep(100);
	/* we should connect to network first. */
	main_cmd_exec("net sta config myssid mypsk");

	OS_MSleep(100);
	main_cmd_exec("net sta enable");
}
~~~

用自己的SSID和PSK替换myssid和mypsk，重启即可；

#### 3、授权流程

- 通过腾讯云叮当手机端SDK生成Client ID；

- 在XR871控制台，输入指令：

  tvs auth xxxxxxx

  xxxxxxx  为Client ID；

  控制台打印：“tvs authorize success” 代表授权成功；

- 通过指令进行授权是测试阶段的流程，产品级流程需要通过手机集成腾讯云小微SDK，并执行授权操作，具体参考接入指南；

#### 4、启动智能语音对话

- 在XR871控制台，输入指令：

  tvs speech

  可以开始执行语音识别流程，对着麦克风说话即可，云端VAD将识别到说话结束的标识，设备端将会有语音回应；

#### 5、播放音乐和播放控制

- 启动语音对话，输入”播放一首歌曲“等语料，将会有歌曲下发，设备端将开始播放；
- 如果设备端TTS回复“账号认证失败”等，一般是因为授权流程不正常；
- 播放过程中启动语音对话，输入“停止播放”等语料，可以停止播放歌曲；
- 播放过程中启动语音对话，输入“上一首“/"下一首”等语料，可以进行播放控制操作；
- XR871平台的各种流媒体格式的支持，参考《XR871播放功能选择使用说明》

#### 6、接入指南

请参考SDK中的《TVS SDK FreeRTOS接入指南》文档