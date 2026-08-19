#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "speex/speex.h"
#include "Config.h"

#define TVS_LOG_DEBUG_MODULE "SPEEX"



#include "tvs_log.h"
#include "cias_log.h"
#include "cias_common.h"
#include "tvs_speex_api.h"
static SpeexBits ebits;
static void *enc_state = NULL;
static bool g_codc_open = false;

int tvs_speex_encode(char *pcm, int pcm_size, char *codec_buffer, int codec_buffer_size)
{
  #if CIAS_AUDIO_SPEEX_ENABLE
  if (!g_codc_open)
  {
    return 0;
  }
  int nbBits = 0;
  speex_bits_reset(&ebits);
  CIAS_LOG_HL("--------------------1start----------------------\r\n");
  /*CIAS_LOG_HL("pcm_size = %d\r\n", pcm_size);
  for (int i = 0; i < pcm_size; i++)
  {
    CIAS_LOG_HL("%02x", pcm[i]);
  }*/
  speex_encode_int(enc_state, (short *)pcm, &ebits);
  nbBits = speex_bits_write(&ebits, codec_buffer + 1, codec_buffer_size);
  codec_buffer[0] = (char)nbBits;
  nbBits++;
  CIAS_LOG_HL("nbBits = %d, codec_buffer_size = %d\r\n", nbBits, codec_buffer_size);
  // for (int i = 0; i < nbBits; i++)
  // {
  //   CIAS_LOG_HL("%x ", codec_buffer[i]);
  // }
  CIAS_LOG_HL("-------------------3end--------------------------\r\n");
  return nbBits;
  #endif
}

int tvs_speex_enc_open(int compress)
{
  #if CIAS_AUDIO_SPEEX_ENABLE
  if (g_codc_open)
  {
    return 0;
  }

  g_codc_open = true;
  speex_bits_init(&ebits);
  enc_state = speex_encoder_init(&speex_wb_mode);
  speex_encoder_ctl(enc_state, SPEEX_SET_QUALITY, &compress);
  int framesize = 0;
  speex_encoder_ctl(enc_state, SPEEX_GET_FRAME_SIZE, &framesize);
  TVS_LOG_PRINTF("speex encoder open, framesize %d, compress %d\n", framesize, compress);
  return 0;
  #endif
}

void tvs_speex_enc_close()
{
  #if CIAS_AUDIO_SPEEX_ENABLE
  if (!g_codc_open)
  {
    return;
  }
  g_codc_open = false;
  speex_bits_destroy(&ebits);
  speex_encoder_destroy(enc_state);
  TVS_LOG_PRINTF("speex encoder close\n");
  #endif
}

//speex test
void tvs_speex_enc_test(void)
{
  #if CIAS_AUDIO_SPEEX_ENABLE
  uint8_t pcm_buf[960] = {0};
  uint8_t codec_buf[960] = {0};
  tvs_speex_enc_open(5);
  for(int i = 0; i < 960; i++)
  {
    pcm_buf[i] = 0x2A + i;
  }
  while (1)
  {
    CIAS_LOG_HL("------tvs_speex_encode------\r\n");
    tvs_speex_encode(pcm_buf, 960, codec_buf, 960);
    cias_task_delay_ms(3000);
  }
  
#endif
}
