#include "cias_ap_net.h"
#include "cias_demo_config.h"
#include "cias_log.h"
#include "cias_freertos_task.h"
#include "cias_freertos_common.h"
#include "lwip/dhcp.h"
#include "lwip/ip.h"
#include "cias_user_config.h"
#include "Config.h"


#if defined(CONFIG_APP_CIAS_CLOUD_TVS)
#include "cias_tvs_upload.h"
#endif
cias_wifi_param_t gcias_wifi_param = {0};

#define CIAS_LOG_LEVEL __LOG_DEBUG
#define CIAS_LOG_MODULE "[AP_NET]"

static int default_sta_connect_event_hook(void);
static int default_sta_disconnect_event_hook(void);
static int default_sta_timeout_hook(void);
static int default_successful_hook(void);
static int default_timeout_hook(void);
//配网钩子函数结构体
static ap_net_t ci_ap_net = {
    .sta_is_connected = 0,
    .start_net = 0,
    .exit_net = 0,
    .is_busy = 0,
    .networking_s = eCI_AP_UDP_INIT_WIFI_S,
    .hook.sta_connect_event = default_sta_connect_event_hook,
    .hook.sta_disconnect_event = default_sta_disconnect_event_hook,
    .hook.sta_timeout = default_sta_timeout_hook,
    .hook.successful = default_successful_hook,
    .hook.timeout = default_timeout_hook,
};
/**
 * @description: 设置wifi连接状态函数, 将触发sta连接状态钩子函数
 * @param {flag:连接状态. 1:已连接 0:连接断开}
 * @return {void}
 */
void cias_set_sta_connect_status(unsigned char flag)
{
  if (ci_ap_net.sta_is_connected != flag)
  {
    if (flag)
    {
      CIAS_LOG_DEBUG("sta connect event");
      ci_ap_net.hook.sta_connect_event();
    }
    else
    {
      CIAS_LOG_DEBUG("sta disconnect event");
      ci_ap_net.hook.sta_disconnect_event();
    }
    ci_ap_net.sta_is_connected = flag;
  }
}
/**
 * @description: 获取wifi连接状态函数
 * @param {void}
 * @return {wifi连接状态. 1:已连接 0:未连接}
 */
unsigned char cias_get_sta_connect_status(void)
{
  return ci_ap_net.sta_is_connected;
}
/**
 * @description: 开始配网函数,执行此函数,配网loop开始配网.
 * @param {void}
 * @return {void}
 */
void cias_start_ap_net(void)
{
  ci_ap_net.start_net = 1;
}
/**
 * @description: 退出配网函数,执行此函数,配网loop退出配网
 * @param {void}
 * @return {void}
 */
void cias_exit_ap_net(void)
{
  ci_ap_net.exit_net = 1;
}
/**
 * @description: 设置配网模式为STA状态,可以进行sta连接
 * @param {type}
 * @return {type}
 */
void cias_udp_recv_data_after_sta_mode(void)
{
  ci_ap_net.networking_s = eCI_STA_INIT_WIFI_S;
}
/**
 * @description: 获取现在是否在配网
 * @param {type}
 * @return {unsigned char:  1:正在配网
 *                          0:空闲}
 */
unsigned char cias_get_net_is_busy(void)
{
  return ci_ap_net.is_busy;
}

/**
 * 默认事件钩子函数
 */
static int default_sta_connect_event_hook(void)
{
  return 0;
}
static int default_sta_disconnect_event_hook(void)
{
  return 0;
}
static int default_sta_timeout_hook(void)
{
  return 0;
}
static int default_successful_hook(void)
{
  return 0;
}
static int default_timeout_hook(void)
{
  return 0;
}
/**
 * @description: 配网流程初始化函数
 * @param { ap_mode:           设置wifi为ap模式函数
 *          udp_server_enter:       udp server初始化函数
 *          udp_server_exit:        udp server退出函数
 *          sta_mode_connect_wifi:  sta模式连接wifi函数
 *          exit_ap_mode:      wifi关闭ap模式函数
 *          exit_sta_mode:     wifi关闭sta模式函数
 *          idle_hook:              空闲钩子函数,当配网loop里面不执行其他操作时候,会循环执行此函数
 *          delay_ms:               配网流程毫秒延时函数
 * }
 * @return {void}
 */
int cias_ap_network_hooks_init(cias_ap_net_hal_t *hal, cias_ap_net_hook_t *hook)
{
  ci_ap_net.sta_is_connected = 0;
  ci_ap_net.start_net = 0;
  ci_ap_net.exit_net = 0;
  ci_ap_net.networking_s = eCI_AP_UDP_INIT_WIFI_S;

  CIAS_ASSERT_RETURN(hal != NULL);
  CIAS_ASSERT_RETURN(hal->ap != NULL);
  CIAS_ASSERT_RETURN(hal->exit_ap != NULL);
  CIAS_ASSERT_RETURN(hal->udp_enter != NULL);
  CIAS_ASSERT_RETURN(hal->udp_exit != NULL);
  CIAS_ASSERT_RETURN(hal->sta != NULL);
  CIAS_ASSERT_RETURN(hal->exit_sta != NULL);
  CIAS_ASSERT_RETURN(hal->delay_ms != NULL);

  ci_ap_net.hal.ap = hal->ap;
  ci_ap_net.hal.exit_ap = hal->exit_ap;
  ci_ap_net.hal.sta = hal->sta;
  ci_ap_net.hal.exit_sta = hal->exit_sta;
  ci_ap_net.hal.udp_enter = hal->udp_enter;
  ci_ap_net.hal.udp_exit = hal->udp_exit;
  ci_ap_net.hal.delay_ms = hal->delay_ms;

  if (hook == NULL)
    return 0;
  if (hook->sta_connect_event != NULL)
    ci_ap_net.hook.sta_connect_event = hook->sta_connect_event;
  if (hook->sta_disconnect_event != NULL)
    ci_ap_net.hook.sta_disconnect_event = hook->sta_disconnect_event;
  if (hook->sta_timeout != NULL)
    ci_ap_net.hook.sta_timeout = hook->sta_timeout;
  if (hook->successful != NULL)
    ci_ap_net.hook.successful = hook->successful;
  if (hook->timeout != NULL)
    ci_ap_net.hook.timeout = hook->timeout;

  return 0;
}
int ip_addr_get_status = 0;
#if TENCENT_AP_NET_WORK_EN//add by hw			
extern int wifi_softAP_connect_flag;
/**
 * @description: 配网流程loop函数
 * @param {void} 
 * @return {void} 
 */
void cias_ap_networking_loop(void)
{
    ap_net_t *p_net = &ci_ap_net;

    while (1)
    {
        int timeout_count = 0;

ci_ap_networking_loop:
        timeout_count = 0;
        p_net->networking_s = eCI_AP_UDP_INIT_WIFI_S;
        
        if (p_net->start_net)
        {
			      CIAS_LOG_HL("AP Networking Start...");
            p_net->start_net = 0;
            p_net->exit_net = 0;
            p_net->is_busy = 1;
            while (1){
                switch (p_net->networking_s){
                    case eCI_AP_UDP_INIT_WIFI_S:{
                        //停止STA模式
						CIAS_LOG_INFO("STA stop...");
                        if(p_net->hal.exit_sta() != 0){
							CIAS_LOG_ERR("STA stop ERR");
                            goto ci_ap_networking_loop;
                        }
                        p_net->hal.delay_ms(1000);
                        //开启AP模式
						CIAS_LOG_HL("AP start...");
                        if(p_net->hal.ap() != 0){
							CIAS_LOG_ERR("AP start ERR");
                            goto ci_ap_networking_loop;
                        }
                        p_net->hal.delay_ms(1000);
                        //开启UDP服务
						CIAS_LOG_INFO("UDP start...");
                        if (p_net->hal.udp_enter() != 0){
							CIAS_LOG_ERR("UDP ERR");
                            p_net->hal.udp_exit();
                            goto ci_ap_networking_loop;
                        }
                        //切换到空闲状态，等待UDP接受SSID和PASSWORD
                        p_net->networking_s = eCI_IDEL_WIFI_S;
                        CIAS_LOG_INFO("Wait UDP server...");
                        break;
                    }
                    case eCI_STA_INIT_WIFI_S:{
                        unsigned int sta_timeout = 0;
                        //退出UDP服务
						CIAS_LOG_INFO("UDP exit");
                        p_net->hal.udp_exit();
                        p_net->hal.delay_ms(100);
                        CIAS_LOG_INFO("close AP");
                        p_net->hal.exit_ap();
                        p_net->hal.delay_ms(1000);
                        //模组sta连接wifi
						CIAS_LOG_HL("STA start...");
                        cias_set_sta_connect_status(0);
                        wifi_stop();
                        wifi_init_sta();
                        wifi_softAP_connect_flag = 0;
                        ip_addr_get_status = 1;
                        goto end;
                        break;
                    }
                    case eCI_IDEL_WIFI_S:{
                        timeout_count++;
                        if (timeout_count < AP_NETWORK_T) //小于AP_NETWORK_T时间的指令不做处理
                            break;
                        if (p_net->start_net || (timeout_count > AP_NETWORK_TIMEOUT) || p_net->exit_net)  //收到开始配网命令、超过五分钟未配网、退出配网命令
                        {
                            //退出UDP服务 ap模式 sta模式
							CIAS_LOG_HL("Close UDP/AP\r\n");
                            p_net->hal.udp_exit();
                            p_net->hal.delay_ms(100);
                            p_net->hal.exit_ap();
                            p_net->hal.delay_ms(1000);
                            if ((timeout_count > AP_NETWORK_TIMEOUT) || p_net->exit_net)  //如果是超时 继续连接存在flash里面的wifi
                            {
                                if (timeout_count > AP_NETWORK_TIMEOUT) 
                                    p_net->hook.timeout();
                                //模组sta连接wifi
								CIAS_LOG_HL("STA start..\r\n");
                                p_net->hal.sta();
                                p_net->hal.delay_ms(1000);
                            }
                            timeout_count = 0;
                            p_net->exit_net = 0;
                            goto ci_ap_networking_loop;
                        }                    
                        break;
                    }
                    default:{
                        p_net->networking_s = eCI_AP_UDP_INIT_WIFI_S;
                        break;
                    }
                }
                p_net->hal.delay_ms(500);
            }
        }
        else
        {
            p_net->is_busy = 0;
        }
        p_net->hal.delay_ms(100);
    }
    end:
		CIAS_LOG_HL("net loop goto end...\r\n");
}
#endif//TENCENT_AP_NET_WORK_EN