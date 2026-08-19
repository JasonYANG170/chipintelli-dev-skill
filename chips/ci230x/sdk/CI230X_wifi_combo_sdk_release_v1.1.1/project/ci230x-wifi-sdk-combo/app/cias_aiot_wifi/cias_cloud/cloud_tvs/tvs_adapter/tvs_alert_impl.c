#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tvs_api.h"
#include "tvs_alert_impl.h"

#include "cJSON.h"

static int tvs_alert_adapter_impl_new(tvs_alert_infos* alerts, int alert_count) {
	// TO-DO 保存闹钟
	return 0;
}

static int tvs_alert_adapter_impl_delete(tvs_alert_summary* alerts, int alert_count) {
	// TO-DO 删除闹钟
	return 0;
}

void* tvs_alert_adapter_impl_get_all_alerts() {
	cJSON* alert_arr = cJSON_CreateArray();
	// TO-DO 获取闹钟概要信息并填充json，必须返回cJSON*类型的数据

	return alert_arr;
}

// TO-DO 闹钟响铃时需要调用此函数通知SDK
void on_alert_trigger_start(const char* alert_token) {
	if (alert_token != NULL) {
		return;
	}

	tvs_alert_adapter_on_trigger(alert_token);
}

// TO-DO 闹钟响铃之后，超时或者手动停止，都需要调用此函数通知SDK
void on_alert_trigger_stop(const char* alert_token, tvs_alert_stop_reason reason) {
	if (alert_token != NULL) {
		return;
	}

	tvs_alert_adapter_on_trigger_stop(alert_token, reason);
}

static void alert_init() {
	
}

int tvs_init_alert_adater_impl(tvs_alert_adapter* ad) {
	if (NULL == ad) {
		return -1;
	}

	// 初始化adapter
	ad->do_delete = tvs_alert_adapter_impl_delete;
	ad->do_new_alert = tvs_alert_adapter_impl_new;
	ad->get_alerts_ex = tvs_alert_adapter_impl_get_all_alerts;
	alert_init();
	return 0;
}

