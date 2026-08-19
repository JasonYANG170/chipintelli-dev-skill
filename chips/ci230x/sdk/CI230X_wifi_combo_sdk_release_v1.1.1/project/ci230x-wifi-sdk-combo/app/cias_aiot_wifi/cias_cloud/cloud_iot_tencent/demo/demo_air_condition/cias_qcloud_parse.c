

#include "cJSON.h"
#include "cias_qcloud_iot.h"
#include "cias_slave_message_handle.h"
#include "qcloud_iot_export_method.h"
#include "qcloud_iot_export_data_template.h"
#include "cias_log.h"

#include "cias_user_config.h"

#if NET_AUDIO_DOWNLOAD_ENABLE
#include "cias_media.h"
#endif

#if defined(APP_CIAS_CLOUD_IOT_TENCENT)
#include "cias_qcloud_parse.h"
#endif
#define MP3_URL_LENGTH 256

#define TOTAL_PROPERTY_COUNT 9

typedef enum
{
	AIR_KEY,
	WIND_SPEED,
	WIND_KEY,
	U_D_WIND,
	L_R_WIND,
	TEMPERATURE,
	MODE,
	TIMING,
	VOICE_URL
}data_index_t;

typedef struct _ProductDataDefine
{
	TYPE_DEF_TEMPLATE_INT air_key;	   //
	TYPE_DEF_TEMPLATE_INT wind_speed;  //
	TYPE_DEF_TEMPLATE_INT wind_key;	   //
	TYPE_DEF_TEMPLATE_INT U_D_wind;	   //
	TYPE_DEF_TEMPLATE_INT L_R_wind;	   //
	TYPE_DEF_TEMPLATE_INT temperature; //
	TYPE_DEF_TEMPLATE_INT mode;		   //
	TYPE_DEF_TEMPLATE_INT timing;	   //
	TYPE_DEF_TEMPLATE_STRING mp3_url[MP3_URL_LENGTH];//
} ProductDataDefine;

static ProductDataDefine sg_product_data;

sDataPoint sg_data_template[TOTAL_PROPERTY_COUNT];

int iot_cmd_json_analytical(cJSON *json, char *object, int *vulue)
{
	cJSON *json_value = NULL;

	json_value = cJSON_GetObjectItem(json, object);
	if (json_value != NULL)
	{
		if (json_value->type == cJSON_Number)
		{
			*vulue = json_value->valueint;
			return 1;
		}
		else if(json_value->type == cJSON_Object)
        {
            cJSON *json_url = cJSON_GetObjectItem(json_value,"mp3_url_addr");
            if(json_url != NULL)
            {
                if(json_url->type == cJSON_String)
                {
                    CIAS_LOG_DEBUG("json_url=%s\n",json_url->valuestring);
                    strcpy((char*)vulue,json_url->valuestring);
                    return 1;
                }
            }
        }
	}
	
	return 0;
	
}
// test mp3 url
//http://m801.music.126.net/20220322165837/afb5c132b0d01ee06c01d69c7096764a/jdymusic/obj/wo3DlMOGwrbDjj7DisKw/11983356173/ed2f/6024/be41/2dc456563c5f9c9535b75ecb066c0325.mp3

int cias_qcloud_down_raw_json(char *control_str)
{
	extern sDataPoint sg_data_template[];
	extern bool sg_control_msg_arrived;
	sDataPoint *p_data_template = NULL;
	cJSON *json = NULL;
	int value = 0;

	json = cJSON_Parse(control_str);
	CIAS_ASSERT_RETURN(json != NULL);
	CIAS_LOG_DEBUG("\r\nrecv iot msg2:%s\n", control_str);
	for (int i = 0; i < TOTAL_PROPERTY_COUNT; i++)
	{
		p_data_template = &(sg_data_template[i]);
		switch (p_data_template->data_property.type)
        {
            case JINT32:
            {
                if (iot_cmd_json_analytical(json, p_data_template->data_property.key, (int32_t*)(p_data_template->data_property.data)) != 0)
                {
                    p_data_template->data_property.state = eCHANGED;
                    sg_control_msg_arrived = true;
                    goto return_goto;
                }
                break;
            }
            case JSTRING:
            {
                if (iot_cmd_json_analytical(json, p_data_template->data_property.key, (int32_t*)(p_data_template->data_property.data)) != 0)
                {
                    p_data_template->data_property.state = eCHANGED;
                    sg_control_msg_arrived = true;
                    goto return_goto;
                }
                break;
            }
        }
	}
return_goto:
	cJSON_Delete(json);
	return 0;
}

void _init_data_template(void)
{
	sg_product_data.air_key = 0;
	sg_data_template[AIR_KEY].data_property.data = &sg_product_data.air_key;
	sg_data_template[AIR_KEY].data_property.key = "air_key";
	sg_data_template[AIR_KEY].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[AIR_KEY].data_property.state = eCHANGED;

	sg_product_data.wind_speed = 0;
	sg_data_template[WIND_SPEED].data_property.data = &sg_product_data.wind_speed;
	sg_data_template[WIND_SPEED].data_property.key = "wind_speed";
	sg_data_template[WIND_SPEED].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[WIND_SPEED].data_property.state = eCHANGED;

	sg_product_data.wind_key = 0;
	sg_data_template[WIND_KEY].data_property.data = &sg_product_data.wind_key;
	sg_data_template[WIND_KEY].data_property.key = "wind_key";
	sg_data_template[WIND_KEY].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[WIND_KEY].data_property.state = eCHANGED;

	sg_product_data.U_D_wind = 0;
	sg_data_template[U_D_WIND].data_property.data = &sg_product_data.U_D_wind;
	sg_data_template[U_D_WIND].data_property.key = "U_D_wind";
	sg_data_template[U_D_WIND].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[U_D_WIND].data_property.state = eCHANGED;

	sg_product_data.L_R_wind = 0;
	sg_data_template[L_R_WIND].data_property.data = &sg_product_data.L_R_wind;
	sg_data_template[L_R_WIND].data_property.key = "L_R_wind";
	sg_data_template[L_R_WIND].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[L_R_WIND].data_property.state = eCHANGED;

	sg_product_data.temperature = 20;
	sg_data_template[TEMPERATURE].data_property.data = &sg_product_data.temperature;
	sg_data_template[TEMPERATURE].data_property.key = "temperature";
	sg_data_template[TEMPERATURE].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[TEMPERATURE].data_property.state = eCHANGED;

	sg_product_data.mode = 0;
	sg_data_template[MODE].data_property.data = &sg_product_data.mode;
	sg_data_template[MODE].data_property.key = "mode";
	sg_data_template[MODE].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[MODE].data_property.state = eCHANGED;

	sg_product_data.timing = 0;
	sg_data_template[TIMING].data_property.data = &sg_product_data.timing;
	sg_data_template[TIMING].data_property.key = "timing";
	sg_data_template[TIMING].data_property.type = TYPE_TEMPLATE_INT;
	sg_data_template[TIMING].data_property.state = eCHANGED;


	sg_product_data.mp3_url[0] = '\0';
    sg_data_template[VOICE_URL].data_property.data = sg_product_data.mp3_url;
    sg_data_template[VOICE_URL].data_property.key = "mp3_url";
    sg_data_template[VOICE_URL].data_property.type = TYPE_TEMPLATE_STRING;
    sg_data_template[VOICE_URL].data_property.data_buff_len = MP3_URL_LENGTH;
    sg_data_template[VOICE_URL].data_property.state = eNOCHANGE;
};

int deal_down_stream(void)
{
	extern sDataPoint sg_data_template[];
	sDataPoint *p_data_template = NULL;
	int i;
	unsigned short play_index = 0;
	for (i = 0; i < TOTAL_PROPERTY_COUNT; i++)
	{
		p_data_template = &(sg_data_template[i]);
		if (p_data_template->data_property.state == eCHANGED)
		{
			break;
		}
	}

	switch (i)
	{
		case AIR_KEY: // air_key
		{
			// play_index = 1006 - *(int32_t *)(p_data_template->data_property.data);
			play_index = 3 - *(int32_t *)(p_data_template->data_property.data);
			break;
		}

		{
		case WIND_SPEED: // wind_speed
			// play_index = 1010 - *(int32_t *)(p_data_template->data_property.data);
			play_index = 29 + *(int32_t *)(p_data_template->data_property.data);
			break;
		}

		case WIND_KEY: // wind_key
		{
			// play_index = 1011 + *(int32_t *)(p_data_template->data_property.data);
			// play_index = 28 + *(int32_t *)(p_data_template->data_property.data);  data = 1,开始扫风    data = 0,停止扫风
			play_index = 28; //标准智能管家只有 停止摆动，没有开始摆动命令
			break;
		}

		case U_D_WIND: // U_D_wind
		{
			// play_index = 1070 - (*(int32_t *)(p_data_template->data_property.data)) * 2;
			if ((*(int32_t *)(p_data_template->data_property.data)) == 1)
				play_index = 27;
			else
				play_index = 28;
			break;
		}

		case L_R_WIND: // L_R_wind
		{
			if ((*(int32_t *)(p_data_template->data_property.data)) == 1)
				play_index = 26;
			else
				play_index = 28;
			break;
		}

		case TEMPERATURE: // temperature
		{

			play_index = 20 + *(int32_t *)(p_data_template->data_property.data);
			break;
		}

		case MODE: // mode
		{
			/*  if (*(int32_t *)(p_data_template->data_property.data) == 5) //省电模式 //data 0（制冷） 1（制热） 2（送风） 3（除湿） 4（自动） 5（省电）
			{
					play_index = 1103;
			} */
			if (*(int32_t *)(p_data_template->data_property.data) == 0)
				play_index = 10;
			if (*(int32_t *)(p_data_template->data_property.data) == 1)
				play_index = 9;
			if (*(int32_t *)(p_data_template->data_property.data) == 2)
				play_index = 11;
			if (*(int32_t *)(p_data_template->data_property.data) == 3)
				play_index = 14;
			if (*(int32_t *)(p_data_template->data_property.data) == 4)
				play_index = 8;
			if (*(int32_t *)(p_data_template->data_property.data) == 5)
				play_index = 12;
			break;
		}

		case TIMING: // timing
		{
			// play_index = 1104 + *(int32_t *)(p_data_template->data_property.data); //data 0(定时0小时) 1(定时1小时) 2(定时2小时) 。。。10(定时10小时)
			if (*(int32_t *)(p_data_template->data_property.data) == 1)
				play_index = 23;
			if (*(int32_t *)(p_data_template->data_property.data) == 2)
				play_index = 24;
			break;
		}
		case VOICE_URL:
		{
#if NET_AUDIO_DOWNLOAD_ENABLE
			char *p = (char *)p_data_template->data_property.data;
            CIAS_LOG_HL("recv iot url:%s\r\n",p);
            
            extern void wakeup_deal(void);
            cias_finish_play_music();
            wakeup_deal();
            cias_media_set_skip_speak(false);
            cias_media_clear_suspension();
            cias_media_set_block_size(0);
            cias_media_quit_connect();
            cias_media_recv_cmd(CIAS_RECV_PLAY_END_CMD);

            cias_media_add_audio_url(p);
            cias_media_set_url_type(ONEMUSIC);            
            cias_media_set_audio_state(MEDIA_AUDIO_PLAY);
            cias_media_set_speak_state(MEDIA_SPEAK_STOP);
            cias_media_recv_cmd(CIAS_CONNECT_URL_CMD);
#endif
            break;
		}

		default:
			break;
	}

	if (play_index != 0)
	{
		CIAS_LOG_HL("recv: %d\n\n", play_index);
		cias_message_send_interface(QCLOUD_IOT_CMD, DEF_FILL, 2, (unsigned char *)(&play_index)); // remove by yjd
	}

	return 0;
}

bool is_recv_play_index = false;
unsigned short recv_slave_cmd_index = 0; //接收到从端指令编号
int deal_up_stream(void)
{
	extern sDataPoint sg_data_template[];
	if (is_recv_play_index == false)
	{
		return 0;
	}

	CIAS_LOG_HL("deal_up_stream: %d\n\n", recv_slave_cmd_index);

	is_recv_play_index = false;
	if ((recv_slave_cmd_index >= 30) && (recv_slave_cmd_index <= 34)) // wind_speed
	{
		*(int32_t *)(sg_data_template[WIND_SPEED].data_property.data) = recv_slave_cmd_index - 29;
		if (recv_slave_cmd_index == 34)
			*(int32_t *)(sg_data_template[WIND_SPEED].data_property.data) = 3;
		sg_data_template[WIND_SPEED].data_property.state = eCHANGED;
	}
	else if ((recv_slave_cmd_index >= 37) && (recv_slave_cmd_index <= 51)) // temperature 16~30度
	{
		*(int32_t *)(sg_data_template[TEMPERATURE].data_property.data) = recv_slave_cmd_index - 20;
		sg_data_template[TEMPERATURE].data_property.state = eCHANGED;
	}
	else if ((recv_slave_cmd_index >= 8) && (recv_slave_cmd_index <= 11)) // mode
	{
		*(int32_t *)(sg_data_template[MODE].data_property.data) = recv_slave_cmd_index - 1025;

		if (recv_slave_cmd_index == 8)
			(*(int32_t *)(sg_data_template[MODE].data_property.data) = 4);
		if (recv_slave_cmd_index == 9)
			(*(int32_t *)(sg_data_template[MODE].data_property.data) = 1);
		if (recv_slave_cmd_index == 10)
			(*(int32_t *)(sg_data_template[MODE].data_property.data) = 0);
		if (recv_slave_cmd_index == 11)
			(*(int32_t *)(sg_data_template[MODE].data_property.data) = 2);

		sg_data_template[MODE].data_property.state = eCHANGED;
	}
	else if ((recv_slave_cmd_index >= 23) && (recv_slave_cmd_index <= 25)) // timing
	{
		if (recv_slave_cmd_index == 23)
			(*(int32_t *)(sg_data_template[TIMING].data_property.data) = 1);
		if (recv_slave_cmd_index == 24)
			(*(int32_t *)(sg_data_template[TIMING].data_property.data) = 2);
		if (recv_slave_cmd_index == 25)
			(*(int32_t *)(sg_data_template[TIMING].data_property.data) = 2);
		sg_data_template[TIMING].data_property.state = eCHANGED;
	}
	else
	{
		switch (recv_slave_cmd_index)
		{
		case 2: // air_key 1
			*(int32_t *)(sg_data_template[AIR_KEY].data_property.data) = 1;
			sg_data_template[AIR_KEY].data_property.state = eCHANGED;
			break;
		case 3: // air_key 0
			*(int32_t *)(sg_data_template[AIR_KEY].data_property.data) = 0;
			sg_data_template[AIR_KEY].data_property.state = eCHANGED;
			break;
		case 28: // wind_key 0
			*(int32_t *)(sg_data_template[WIND_KEY].data_property.data) = 0;
			sg_data_template[WIND_KEY].data_property.state = eCHANGED;
			break;
		/* case 28: //wind_key 1
				*(int32_t *)(sg_data_template[WIND_KEY].data_property.data) = 1;
				sg_data_template[WIND_KEY].data_property.state = eCHANGED;
				break; */
		case 27: // U_D_wind 1
			*(int32_t *)(sg_data_template[U_D_WIND].data_property.data) = 1;
			sg_data_template[U_D_WIND].data_property.state = eCHANGED;
			break;
		/* case 1070: //U_D_wind 0
				*(int32_t *)(sg_data_template[U_D_WIND].data_property.data) = 0;
				sg_data_template[U_D_WIND].data_property.state = eCHANGED;
				break; */
		case 26: // L_R_wind 1
			*(int32_t *)(sg_data_template[L_R_WIND].data_property.data) = 1;
			sg_data_template[L_R_WIND].data_property.state = eCHANGED;
			break;
		/* case 1071: //L_R_wind 0
				*(int32_t *)(sg_data_template[L_R_WIND].data_property.data) = 0;
				sg_data_template[L_R_WIND].data_property.state = eCHANGED;
				break;
		case 1103: //省电模式
				*(int32_t *)(sg_data_template[MODE].data_property.data) = 5;
				sg_data_template[MODE].data_property.state = eCHANGED;
				break;*/
		default:
			return 0;
		}
	}

	return 1;
}

int recv_audio_cmd_handle(unsigned short cmd)
{
	CIAS_LOG_HL("recv audio cmd:%d", cmd);

	//提供给qcloud上传
	is_recv_play_index = true;
	recv_slave_cmd_index = cmd;

	/*
	//提供给配网
	if (cmd == 1115) //进入配网
	{
		cias_start_ap_net();
	}
	else if (cmd == 1116) //退出配网
	{
		cias_exit_ap_net();
	}
	*/
	return 0;
}
