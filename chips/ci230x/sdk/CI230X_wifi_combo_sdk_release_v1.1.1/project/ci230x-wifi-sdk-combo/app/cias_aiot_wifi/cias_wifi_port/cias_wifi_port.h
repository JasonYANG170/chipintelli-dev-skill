
#include <stdint.h>

typedef enum {
    WSS_IDLE = 0,                       ///< not connected
    WSS_CONNECTING,                     ///< connecting wifi
    WSS_PASSWD_WRONG,                   ///< passwd not match
    WSS_NO_AP_FOUND,                    ///< ap is not found
    WSS_CONN_FAIL,                      ///< connect fail
    WSS_CONN_SUCCESS,                   ///< connect wifi success
    WSS_GOT_IP,                         ///< get ip success
}WF_STATION_STAT_E;

typedef enum
{
    CIAS_WIFI_STA_CONNECTED = 0x01,
    CIAS_WIFI_STA_DISCONNECTED,
    CIAS_WIFI_STA_ERROR,
}cias_wifi_sta_state_t;

typedef struct 
{
    uint8_t wifi_sta_connect_state;  //wifi sta模式配网
}cias_wifi_sta_param_t;

void cias_adapter_wifi_init(void);
void cias_set_wifi_sta_connect_state(int state );
int cias_get_wifi_sta_connect_state( );
void wifi_conn_listen(void);