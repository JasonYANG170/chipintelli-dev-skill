
#include "cias_wifi_port.h"
#include "hal/hal_adc.h"
#include "drv_adc_measure.h"
#include "netif/ethernetif.h"
#include "wifi.h"
#include "wifi_port.h"
#include "wifi_manager.h"
#include "ln_kv_api.h"
#include "ln_psk_calc.h"
#include "ln_nvds.h"
#include "ln_utils.h"
#include "utils/debug/log.h"
#include "utils/system_parameter.h"
#include "utils/power_mgmt/ln_pm.h"
#include "lwip/inet.h"
#include "lwip/netif.h"
#include "cias_freertos_task.h"
#include "cias_common.h"
#include "cias_slave_message_handle.h"
cias_wifi_sta_param_t g_cias_wifi_sta_param;


void cias_adapter_wifi_init(void)
{
    g_cias_wifi_sta_param.wifi_sta_connect_state = CIAS_WIFI_STA_DISCONNECTED;  
}

void cias_set_wifi_sta_connect_state(int state )
{   
    g_cias_wifi_sta_param.wifi_sta_connect_state = state;
}

int cias_get_wifi_sta_connect_state( )
{   
    g_cias_wifi_sta_param.wifi_sta_connect_state = netdev_get_link_state(netdev_get_active());
    return g_cias_wifi_sta_param.wifi_sta_connect_state;
}

void cias_set_wifi_net_state(int state)
{
#if defined(CONFIG_APP_CIAS_CLOUD_TVS)
  set_tvs_network_state(state);
#endif
  g_cias_wifi_sta_param.wifi_sta_connect_state = state;
}

void wifi_conn_listen_task(void *parameter)
{

  while (1)
  {
    cias_get_wifi_sta_connect_state();
    if (g_cias_wifi_sta_param.wifi_sta_connect_state == NETDEV_LINK_UP)
    {
#if CIAS_IOT_TVS_ENABLE
      if (!get_tvs_network_state())
      {
        set_tvs_network_state(true);
        CIAS_LOG_HL("TVS connecting!\r\n");
      }

#endif
    }

    if (g_cias_wifi_sta_param.wifi_sta_connect_state == NETDEV_LINK_DOWN)
    {
#if CIAS_IOT_TVS_ENABLE
      if (get_tvs_network_state())
      {
        cias_set_wifi_net_state(false);
        CIAS_LOG_HL("TVS disconnecting!\r\n");
        // cias_message_send_interface(SEND_WIFI_DISCONNECTED_CMD, DEF_FILL, 0, NULL);
      }

#endif
    }
    // extern void cias_heap_info(void);
    // cias_heap_info();
    
    cias_task_delay_ms(5000);
  }
}
void wifi_conn_listen(void)
{
  int ret = 0;
  static TaskHandle_t wifi_listen_task_handle;
  cias_task_create(&wifi_listen_task_handle, "wifi_conn-listen-task", (cias_task_function_t)wifi_conn_listen_task, (void *)0, CIAS_APP_PRIORITY, (unsigned short)1024 * 1);
}


