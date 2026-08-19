<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI110X%E8%8A%AF%E7%89%87SDK/components/LWIP/ -->

# TCP/IP协议栈(LWIP)

---

## 1. 概述

lwIP是TCP/IP协议的移植轻量级实现，实现的重点是减少资源使用同时又具有全面的TCP功能。主要功能包括：

* 协议：IP，IPv6，ICMP，ND，MLD，UDP，TCP，IGMP，ARP，PPPoS，PPPoE协议；
* DHCP客户端，DNS客户端，AutoIP/APIPA，SNMP agent；
* APIs：增强型专用API，BSD socket API；
* 扩展功能：IP转发，TCP阻塞控制，RTT估算，快速恢复；
* Addon应用程序：HTTP(s)服务器，SNTP客户端，SMTP(s)客户端，ping，NetBIOS服务器，mDNS响应器，MQTT客户端，TFTP服务器。

目前CI110X系列芯片SDK中移植了lwip1.4.1版本，针对 Realtek 8189FTV sdio wifi 驱动和 MARVELL 88w8801 sdio wifi驱动完成了对接，在完成wifi模块的无线网络连接后，可以直接使用BSD socket接口进行网络编程。

提示

更多lwip信息请查看 ☞ [lwip官网](https://savannah.nongnu.org/projects/lwip/)  获取帮助。

---

## 2. 使用说明

lwip的初始化函数已经进行了封装，仅需要调用LwIP\_Init即可完成协议栈的初始化操作，但要注意的是，此时无线网卡未必已经连接到AP，所以lwip初始化时网卡默认是linkdown的，如需连接到网络仍需调用相关wifi驱动函数，如下示例：

* Realtek 8189FTV sdio wifi 网络初始化

```
/* Realtek 8189FTV sdio wifi 网络初始化 */
/* 初始化WiFi */
ret = wifi_on(RTW_MODE_STA);
ci_loginfo(LOG_DUERAPP,"wifi on success!\n");

/* 初始化网络协议栈 */
LwIP_Init();
ci_loginfo(LOG_DUERAPP,"lwip init success!\n");

/* 关联热点 */
ret = wifi_connect(TEST_SSID, RTW_SECURITY_WPA2_AES_PSK, TEST_PASSWORD, strlen(TEST_SSID), strlen(TEST_PASSWORD), 0, NULL);
ci_loginfo(LOG_DUERAPP,"Connected success!\n");

/* 获取ip地址 */
LwIP_DHCP(0, DHCP_START);
ci_loginfo(LOG_DUERAPP,"Got IP success!\n");
```

* MARVELL 88w8801 sdio wifi 网络初始化

```
/* MARVELL 88w8801 sdio wifi 网络初始化 */
/* 初始化wifi */
WiFi_Init();
ci_loginfo(LOG_DUERAPP,"wifi on success!\n");

/*连接到热点*/
WiFi_Connect(TEST_SSID, TEST_PASSWORD, WIFI_AUTH_MODE_OPEN, -1);
ci_loginfo(LOG_DUERAPP,"Connected success!\n");

/* 初始化lwip协议栈 */
LwIP_Init_marvell();
ci_loginfo(LOG_DUERAPP,"lwip init success!\n");

/* 创建WiFi中断接收任务 */
wifi_irq_thread_create();
/* 获取ip地址 */
LwIP_DHCP_Start_marvell();
ci_loginfo(LOG_DUERAPP,"Got IP success!\n");
```