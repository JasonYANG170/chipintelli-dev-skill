#ifndef __CIAS_AUDIO_DATA_HANDLE_H__
#define __CIAS_AUDIO_DATA_HANDLE_H__
#include "ci130x_scu.h"
#include "ci130x_uart.h"
#include "user_config.h"

bool cias_online_func_init(void);
#if AUDIO_DATA_PLAY_BY_UART
bool audio_player_param_init(void);
void request_play_data_func(void);
#endif
#endif   //__CIAS_AUDIO_DATA_HANDLE_H__
