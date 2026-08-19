<!-- Source: https://document.chipintelli.com/%E8%BD%AF%E4%BB%B6%E5%BC%80%E5%8F%91/SDK/CI230X%E8%8A%AF%E7%89%87SDK/start/AT%E6%8C%87%E4%BB%A4%E9%9B%86/ -->

# AT指令集

## 1. 指令简介

​ 下面介绍CI230X系列芯片 WIFI端支持的AT指令集，这里只列举简单常用的一些指令，不常用的AT指令可以到SDK源码

project\components\ln\_at\_cmd\wifi\ln\_at\_cmd\_wifi.c中查询，同时用户也可以根据需求参照ln\_at\_cmd\_wifi.c文件中实现AT指令的流程自行添加新的AT指令。

| AT指令 | 指令说明 |
| --- | --- |
| AT+CIPSTAMAC=”1A:AF:C2:5E:DF:E2” | 设置设备MC地址 |
| AT+CIPSTAMAC? | 查询设备MAC地址 |
| AT+CWLAP | 扫描路由热点 |
| AT+CWJAP=”AIOT\_TEST”,”12345678” | 连接路由 |
| AT+CIPSTA? | 查询设备IP地址 |
| AT+RST | 系统软复位 |
| AT+PING=”192.168.1.1”,”-i”,1024 | PING路由器 |
| AT+TENCENT\_TVS\_PROFILE=”xxxx” | 腾讯TVS鉴权文件设置 |
| AT+TENCENT\_TVS\_PROFILE? | 腾讯TVS鉴权文件获取 |
| AT+QCLOUD\_IOT\_PROFILE=”xxxx” | 腾讯IOT鉴权文件设置 |
| AT+QCLOUD\_IOT\_PROFILE? | 腾讯IOT鉴权文件获取 |
| AT+TUYA\_IOT\_PROFILE==”xxxx” | 涂鸦IOT鉴权文件设置 |
| AT+TUYA\_IOT\_PROFILE？ | 涂鸦IOT鉴权文件获取 |

## 2. AT指令实现说明

### 2.1 AT+TENCENT\_TVS\_PROFILE=”xxxx”

```
//获取腾讯tvs鉴权文件
ln_at_err_t ln_at_get_tvs_auth_profile(const char *name)
{
  uint8_t tvs_profile[TVS_PROFILE_MAX_LEN] = {0};

  CIAS_LOG_HL("-ln_at_set_tvs_auth_profile name = %s\r\n", name);

  if(sysparam_tvs_profile_get(tvs_profile) != SYSPARAM_ERR_NONE)
  {
    goto __exit;
  }
  ln_at_printf("%s:%s \r\n", name, tvs_profile);
  ln_at_printf(LN_AT_RET_OK_STR);
  return LN_AT_ERR_NONE;
__exit:
  ln_at_printf(LN_AT_RET_ERR_STR);
  return LN_AT_ERR_COMMON;
}
```

### 2.2 AT+TENCENT\_TVS\_PROFILE?

```
//设置腾讯tvs鉴权文件
ln_at_err_t ln_at_set_tvs_auth_profile(int para_num, const char *name)
{
    LN_UNUSED(para_num);
    LN_UNUSED(name);
    bool is_default = false;
    uint8_t para_index = 1;
    char *str_param = NULL;
    if(LN_AT_PSR_ERR_NONE != ln_at_parser_get_str_param(para_index++, &is_default, &str_param))
    {
        goto __exit;
    }
    if (is_default || !str_param)
    {
        goto __exit;
    }
    if (para_num != 1)
    {
        goto __exit;
    }
    CIAS_LOG_HL("%s:%s\r\n", name, str_param);
    if(sysparam_tvs_profile_update(str_param) != SYSPARAM_ERR_NONE)
    {
       goto __exit;
    }
    ln_at_printf(LN_AT_RET_OK_STR);
    return LN_AT_ERR_NONE;
__exit:
    ln_at_printf(LN_AT_RET_ERR_STR);
    return LN_AT_ERR_COMMON;
}
```

### 2.3 将实现的AT指令注册到系统中

```
LN_AT_CMD_REG(TENCENT_TVS_PROFILE, ln_at_get_tvs_auth_profile, ln_at_set_tvs_auth_profile, NULL, NULL);
```