#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_api.h"
#include "tvs_auth_manager.h"
#include "tvs_api_config.h"
#include "lwip/netif.h"
#include "cias_slave_message_handle.h"
#include "cias_freertos_debug.h"
#include "cias_user_config.h"
#include "cias_ap_net.h"
//#define ID_BASE 0xA20A0610
//#define ID_BYTE_LENGTH 8

unsigned char sta_mac[12] = {0};

//unsigned char id[ID_BYTE_LENGTH * 2] = {0};

extern struct netif *netif_default;

static void save_auth_info(char *account_info, int len)
{
  // TO-DO 将account信息持久化，以便在之后每次重启时初始化SDK
  // account_info是个Json字符串，len为字符串长度+1
}

static char *load_auth_info(int *len)
{
  // TO-DO 读取之前持久化的account信息，如果是首次开机或者未配对，account信息返回NULL，终端将根据传入的各种ID/DSN信息生成新的account信息
  return NULL;
}

static void my_authorize_callback(bool ok, char *account_info, int len, const char *client_id, int error)
{
  if (ok)
  {
    TVS_ADAPTER_PRINTF("tvs authorize success\n");
    save_auth_info(account_info, len);
  }
  else
  {
    // 授权失败
    TVS_ADAPTER_PRINTF("tvs authorize failed\n");
  }
}

static void process_authorize_ret(int ret)
{
  if (ret == 0)
  {
    // 开始执行授权，授权结果将通过my_authorize_callback回调
  }
  else if (ret == TVS_API_ERROR_NETWORK_INVALID)
  {
    // 网络未连接，在网络重连之后会自动启动授权
  }
  else
  {
    // 未发起授权
    my_authorize_callback(false, NULL, 0, "", ret);
  }
}

void init_authorize_guest(const char *produce_id, const char *dsn)
{
  int auth_len = 0;
  // 加载之前存储的授权信息，可以返回NULL
  char *authorize_info = load_auth_info(&auth_len);

  tvs_authorize_manager_initalize(produce_id, dsn,
                                  authorize_info, auth_len, my_authorize_callback);
  // 发起访客授权
  int ret = tvs_authorize_manager_guest_login();

  process_authorize_ret(ret);
}

void init_authorize_normal_Bak(const char *produce_id, const char *dsn)
{
  int auth_len = 0;
  // 加载之前存储的授权信息，可以返回NULL
  char *authorize_info = load_auth_info(&auth_len);

  tvs_authorize_manager_initalize(produce_id, dsn,
                                  authorize_info, auth_len, my_authorize_callback);

  if (authorize_info == NULL || auth_len == 0)
  {
    // 未授权，需要与手机APP连接并下发client ID，再发起设备授权
    TVS_ADAPTER_PRINTF("this device is not authorized\n");
    return;
  }
  // 发起设备授权
  int ret = tvs_authorize_manager_login();

  process_authorize_ret(ret);
}

//从flash中读取鉴权文件
void get_tvs_profile_frome_flash(void)
{
  int ret = 0;
  char tvs_profile_str_b[512];
  cias_task_delay_ms(1000);
  
  CIAS_LOG_HL("strlen(CLOUD_TVS_DEFAULT_PROFILE)  = %d\r\n", strlen(CLOUD_TVS_DEFAULT_PROFILE));
  
  cias_set_auth_file((uint8_t *)CLOUD_TVS_DEFAULT_PROFILE, strlen(CLOUD_TVS_DEFAULT_PROFILE));
}
extern char tvs_client_id[250]; //edit 20210930

void init_authorize_normal(const char *produce_id, const char *dsn)
{
  int auth_len = 0;
  // 加载之前存储的授权信息，可以返回NULL
  char *authorize_info = load_auth_info(&auth_len);
  printf("tvs_authorize_manager_initalize  start \r\n");
  tvs_authorize_manager_initalize(produce_id, dsn,
                                  authorize_info, auth_len, my_authorize_callback);
  printf("tvs_authorize_manager_initalize  end \r\n");

  if (authorize_info == NULL || auth_len == 0)
  {
    // 未授权，需要与手机APP连接并下发client ID，再发起设备授权
    while (strlen(tvs_client_id) == 0)
    {
      cias_task_delay_ms(2000);
      printf("CLOUD_TVS_DEFAULT_PROFILE = %s\r\n", CLOUD_TVS_DEFAULT_PROFILE);
      cias_auth_set_client_id(CLOUD_TVS_DEFAULT_PROFILE);
      //get_tvs_profile_frome_flash();
    }
    printf("client_id%d  %s \r\n", strlen(tvs_client_id), tvs_client_id);

    memcpy((char *)tvs_client_id + strlen(tvs_client_id), sta_mac, 12);

    printf("client_id  %s \r\n", tvs_client_id);
    tvs_authorize_manager_set_client_id(tvs_client_id);
    // **************测试用****************
  }
  // 发起设备授权
  printf("tvs_authorize_manager_login \r\n");
  wifi_conn_listen();  //WiFi连接任务监控
  int ret = tvs_authorize_manager_login();
  printf("process_authorize_ret \r\n");
  process_authorize_ret(ret);
}

void init_authorize_on_boo_bakt()
{
  // TO-DO 请填写本产品的product_id，格式为Appkey:AccessToken
  const char *product_id = "Your_Appkey:Your_AccessToken";
  // TO-DO 请填写设备的DSN
  const char *dsn = "Your_DSN";

  if (CONFIG_USE_GUEST_AUTHORIZE)
  {
    // 访客授权
    init_authorize_guest(product_id, dsn);
  }
  else
  {
    // 设备授权
    init_authorize_normal(product_id, dsn);
  }
}

uint8_t system_mac[] = {0x18, 0xee, 0xdd, 0xff, 0xff, 0x0f}; //change by yjd
void init_authorize_on_boot()
{
  // TO-DO 请填写本产品的produce_id
  const char *produce_id = "feb711004ae411e9b83f6713e1d602f3:db4eee2b600a49b3b7ee3e764513c61c";

  for (char i = 0; i < 6; i++)
  {
    sprintf((unsigned char *)(&sta_mac[2 * i]), "%02x", system_mac[i]);
  }
  const char *dsn = sta_mac; //"8989898989880000";

  printf("dsnt2 = %s \r\n", dsn);

  if (CONFIG_USE_GUEST_AUTHORIZE)
  {
    // 访客授权
    init_authorize_guest(produce_id, dsn);
  }
  else
  {
    // 设备授权
    printf("init_authorize_normal \r\n");
    init_authorize_normal(produce_id, dsn);
  }
}

// 新账号授权
void start_authorize_with_client_id(const char *new_client_id)
{
  if (new_client_id == NULL || strlen(new_client_id) == 0)
  {
    return;
  }

  if (!CONFIG_USE_GUEST_AUTHORIZE)
  {
    // 首先注销
    tvs_authorize_manager_logout();
    // 重新设置账号
    tvs_authorize_manager_set_client_id(new_client_id);
    // 发起设备授权
    int ret = tvs_authorize_manager_login();

    process_authorize_ret(ret);
  }
}
