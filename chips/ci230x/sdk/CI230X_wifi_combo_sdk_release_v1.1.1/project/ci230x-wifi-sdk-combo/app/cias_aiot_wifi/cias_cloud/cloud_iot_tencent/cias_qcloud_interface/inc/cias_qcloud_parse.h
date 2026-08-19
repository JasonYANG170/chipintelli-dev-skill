#ifndef __CI_QCLOUD_PARSE_H_
#define __CI_QCLOUD_PARSE_H_


int cias_qcloud_down_raw_json(char * control_str);
int deal_down_stream(void);
int deal_up_stream(void);
int recv_audio_cmd_handle(unsigned short cmd);
void _init_data_template(void);


#endif



