/*
 * @Description: 
 * @Author: liutq
 * @Date: 2021-11-29 11:01:41
 * @LastEditTime: 2021-12-10 17:19:24
 * @LastEditors: hongchuan.wu
 * @Reference: 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "os_wrapper.h"
#include "speex/speex.h"
#define TVS_LOG_DEBUG_MODULE  "SPEEX"

#include "tvs_log.h"

#include "tvs_audio_codec.h"
#include "tvs_speex_codec.h"
#include "tvs_speex_api.h"

static tvs_audio_codec g_speex_codec = {0};

#define SPEEX_ENCODE_SIZE     960 //add by whc 640 ->960  16K*16*1/8 = 32KB 32KBps*0.03s = 960 
#define SPEEX_TARGET_BUFFER_SIZE     100

int tvs_speex_codec_init(int compress)
{
	compress = compress;
	if (compress < 4) {
		compress = 4;
	} else if (compress > 9) {
		compress = 9;
	}

	g_speex_codec.src_buffer_size = SPEEX_ENCODE_SIZE;
	g_speex_codec.codec_buffer_size = SPEEX_TARGET_BUFFER_SIZE;
	g_speex_codec.do_open = tvs_speex_enc_open;
	g_speex_codec.do_close = tvs_speex_enc_close;
	g_speex_codec.do_encode = tvs_speex_encode;
	g_speex_codec.compress = compress;
	return 0;
}

tvs_audio_codec* tvs_get_speex_codec() {
	return &g_speex_codec;
}

