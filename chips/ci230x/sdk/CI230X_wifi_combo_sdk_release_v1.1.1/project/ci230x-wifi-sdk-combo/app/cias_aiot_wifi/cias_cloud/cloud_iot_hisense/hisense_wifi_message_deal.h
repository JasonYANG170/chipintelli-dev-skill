
#ifndef __CIAS_HISENSE_WIFI_NETWORK_H__
#define __CIAS_HISENSE_WIFI_NETWORK_H__

#include "wifi.h"

int hisense_ap_data_write(ap_info_t *ap_info);

void ci_hisense_TCP_server_enter(void);
void ci_hisense_TCP_server_exit(void);

#endif