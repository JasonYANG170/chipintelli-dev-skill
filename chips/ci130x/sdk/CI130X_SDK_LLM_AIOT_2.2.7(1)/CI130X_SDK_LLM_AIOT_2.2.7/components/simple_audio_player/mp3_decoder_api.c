#include "user_config.h"
#if	SIMPLE_AUDIO_PLAYER_ENABLE
#include "ci_log.h"
#include "romlib_runtime.h"
#include "simple_audio_player.h"

#if MP3_NO_CONSTANT_BITRATE
/*采样率映射表 hz*/
static int32_t mpeg_1_sample_table[4] = {44100,48000,32000,0};
static int32_t mpeg_2_sample_table[4] = {22050,24000,16000,0};
/*比特率映射表 kbps*/
static int16_t mpeg_1_birate_table[16] = {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320};
static int16_t mpeg_2_birate_table[16] = {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160};

typedef struct {
    unsigned int emphasis      : 2;  // bits 1-0   强调模式 
    unsigned int original      : 1;  // bit 2      原版标志
    unsigned int copyright     : 1;  // bit 3      版权位
    unsigned int mode_ext      : 2;  // bits 5-4   模式扩展 
    unsigned int mode          : 2;  // bits 7-6   声道模式
    unsigned int private_bit   : 1;  // bit 8      私有位
    unsigned int padding       : 1;  // bit 9      填充位
    unsigned int sampling      : 2;  // bits 11-10 采样率索引 
    unsigned int bitrate       : 4;  // bits 15-12 比特率索引
    unsigned int protection    : 1;  // bit 16     保护位
    unsigned int layer         : 2;  // bits 18-17 层
    unsigned int version       : 2;  // bits 20-19 版本 
    unsigned int sync          : 11; // bits 31-21 同步字
} MP3HeaderBits;

typedef union {
    uint32_t raw;
    MP3HeaderBits bits;
} MP3HeaderUnion;
#endif
#pragma push
#pragma pack(1)
typedef struct
{
    char ID3[3];               //"ID3"
    char ver;                  //3
    char revision;             //0
    char flag;                 //0
    uint32_t total_frame_size; //标签帧大小
    char frame_ID[4];          //"PRIV"
    uint32_t frame_size;       //PRIV大小
    uint16_t frame_flag;       //0
    char CI[2];                //"CI"
    uint32_t file_size;        //文件大小
    uint32_t pcm_size;         //PCM大小
}ci_mp3_header_t;
#pragma pop




static void *mp3_decoder_init(void)
{
    // return (void *)MP3InitDecoder();
    return MASK_ROM_LIB_FUNC->mp3func.MP3InitDecoder_p();	
}

static void mp3_docoder_deinit(void* decoder_haldle)
{
    MASK_ROM_LIB_FUNC->mp3func.MP3FreeDecoder_p(decoder_haldle);
}

extern uint32_t total_bytes ;
extern int id3v2_size;
extern uint32_t gap_index;
static int get_info(void* decoder_handle, uint8_t *data, int *bytes_left, uint8_t *pcm_buf, audio_format_info_t *format_info)
{
    int ret = 0;
    static uint8_t err_cnt = 0;
    do
    {
        int32_t mp3_sync_offset;
        format_info->src_data_size = ((ci_mp3_header_t *)data)->file_size;
        /*判断是否有ID3头*/
        if((((ci_mp3_header_t *)data)->ID3[0] == 0x49 
        && ((ci_mp3_header_t *)data)->ID3[1] == 0x44 
        && ((ci_mp3_header_t *)data)->ID3[2] == 0x33)
        ||  id3v2_size != 0)
        {
            uint32_t tmp = ((ci_mp3_header_t *)data)->total_frame_size;
            if (id3v2_size == 0)
            {
                id3v2_size = ((tmp & 0xFF) << 21) | ((tmp & 0xFF00) << 6) | ((tmp & 0xFF0000) >> 9) | ((tmp & 0xFF000000) >> 24);
                // ci_logwarn(LOG_AUDIO_PLAY, "id3v2 size:%x\n", id3v2_size);
            }
            /*跳过ID3头*/
            if(err_cnt == 0)
            {
                total_bytes += *bytes_left;
                // ci_logwarn(LOG_AUDIO_PLAY, "total_bytes size:%x\n", total_bytes);
                /*未读取完的ID3头，返回继续读取数据 ID3的开头为10字节*/
                if (total_bytes < (id3v2_size + 10))
                {            
                    /*ID3最后一包数据来了后，需要gap包中的ID3数据 */
                    gap_index = id3v2_size + 10 - total_bytes;
                    *bytes_left = 0;
                    ret = -1;
                    break;
                }
                /*若第一包就读完ID3头，则直接gap ID3的size*/
                if(gap_index == 0)
                {
                    gap_index = id3v2_size;
                }
                /*计算当前剩余的数据*/
                total_bytes -= id3v2_size;
                *bytes_left = total_bytes;
                // gap_index = 0;
                // total_bytes = 0x200;
                // ci_logwarn(LOG_AUDIO_PLAY, "total_bytes size:%x\n", total_bytes);
                // ci_logwarn(LOG_AUDIO_PLAY, "gap_index size:%x\n", gap_index);
                mp3_sync_offset = MASK_ROM_LIB_FUNC->mp3func.MP3FindSyncWord_p(data + gap_index, total_bytes); // Find the head flag of the frist frame.                
            }
            /*出现错帧则说明已经跳过了ID3头了，继续解码*/
            else
            {
                mp3_sync_offset = MASK_ROM_LIB_FUNC->mp3func.MP3FindSyncWord_p(data, *bytes_left); // Find the head flag of the frist frame.    
            }
        }
        else
        {
            mp3_sync_offset = MASK_ROM_LIB_FUNC->mp3func.MP3FindSyncWord_p(data, *bytes_left); // Find the head flag of the frist frame.
        }
        if (mp3_sync_offset == -1)
        {
            ci_logwarn(LOG_AUDIO_PLAY, "==MP3FindSyncWord_p err!\n");
            ret = -2;
            break;
        }
        // mprintf("mp3_sync_offset = %d\r\n",mp3_sync_offset);
        *bytes_left -= mp3_sync_offset;
        // Decode a frame to get some information about the audio, such as sample rate, channels, output samples per frame.
        uint8_t *in_data_ptr = data + gap_index + mp3_sync_offset;
        // for(int i =0;i<16;i++)
        // {
            
        //     mprintf("data[%d]= %x\r\n",i,data[gap_index + mp3_sync_offset+i]);
        // }
        int32_t err = MASK_ROM_LIB_FUNC->mp3func.MP3Decode_p(decoder_handle, &in_data_ptr, bytes_left, NULL, 0); // Decode a frame.
        MP3FrameInfo mp3FrameInfo;
        MASK_ROM_LIB_FUNC->mp3func.MP3GetLastFrameInfo_p(decoder_handle, &mp3FrameInfo); // Get information about the frame that just decoded.
        if (ERR_MP3_NONE != err)
        {
            err_cnt++;
            gap_index = 0;
            //ci_logwarn(LOG_AUDIO_PLAY, "ERR_MP3_err!\n");
            if(err_cnt >3)
            {
                ci_logwarn(LOG_AUDIO_PLAY, "mp3_decorde err %d,bad frame!\n", err);
                ret = -3;
                err_cnt = 0;
            }
            else
            ret = -1;
            break;
        }
        err_cnt = 0;
        // format_info->pcm_data_size = ((ci_mp3_header_t*)data)->pcm_size*mp3FrameInfo.bitsPerSample/8;
        format_info->channels = mp3FrameInfo.nChans;
        format_info->samprate = mp3FrameInfo.samprate;
        format_info->bits_per_sample = mp3FrameInfo.bitsPerSample;
        format_info->samples_per_frame = mp3FrameInfo.outputSamps;
    } while (0);

    return ret;
}
static int32_t mp3_sync_offset =0;
static int decode_one_frame(void* decoder_handle, uint8_t *data, int *bytes_left, uint8_t *pcm_buf, uint32_t *pcm_data_size)
{
    int ret = 0;
    /*如果非恒定比特率则不用再解析帧头了，已经预解析得到mp3_sync_offset了*/
    #if !MP3_NO_CONSTANT_BITRATE
    mp3_sync_offset = MASK_ROM_LIB_FUNC->mp3func.MP3FindSyncWord_p(data, *bytes_left);   // Find the head flag of the frist frame.
    if (mp3_sync_offset == -1)
    {
        ret = 2;
        return ret;
    }
    #endif
    *bytes_left -= mp3_sync_offset;
    uint8_t * in_data_ptr = data + mp3_sync_offset;
    ret = MASK_ROM_LIB_FUNC->mp3func.MP3Decode_p(decoder_handle, &in_data_ptr, bytes_left, (void*)pcm_buf, 0);  // Decode a frame. 

    return ret;
}
#if MP3_NO_CONSTANT_BITRATE

/**
 * @brief 根据MP3帧头获取当前帧的大小，适配VBR、CBR、ABR三种模式
 * @param data MP3数据地址
 * @param bytes_left MP3数据长度
 * MPEG-1 Layer III：每帧 1152 个采样点
 * MPEG-2 Layer III（含 MPEG-2 LSF）：每帧 576 个采样点
 * 帧大小（字节）= (576或者1152)/8 × 采样率 (kHz)/比特率 (kbps) + 填充位
 * 
​ * 示例1 FF F3 88 C4 
 * 72 × 64/16 = 72 × 4 = 288字节
​ * 示例2 FF F3 28 C4 
 * 72 × 16/16 = 72 × 1 = 72字节
 * @return 当前帧大小
 */
int32_t get_cur_frame_size( uint8_t *data, int *bytes_left)
{
    int32_t ret = 0;
    mp3_sync_offset = MASK_ROM_LIB_FUNC->mp3func.MP3FindSyncWord_p(data, *bytes_left); 
    if (mp3_sync_offset == -1)
    {
        ret = -1;
        return ret;
    }
    uint8_t * in_data_ptr = data + mp3_sync_offset;

    uint32_t big_endian_value = ((uint32_t)in_data_ptr[0]<<24) | ((uint32_t)in_data_ptr[1]<<16) | ((uint32_t)in_data_ptr[2]<<8) | (uint32_t)in_data_ptr[3];
    MP3HeaderUnion header;
    header.raw = big_endian_value;
    // mprintf("big_endian_value      = 0x%4X\n", big_endian_value);
   
    // mprintf("sync      = 0x%03X\n", header.bits.sync);       // 应为 0x7FF (11个1)
    // mprintf("version   = %u\n", header.bits.version);
    // mprintf("layer     = %u\n", header.bits.layer);
    // mprintf("protection= %u\n", header.bits.protection);
    // mprintf("bitrate   = %u\n", header.bits.bitrate);
    // mprintf("sampling  = %u\n", header.bits.sampling);
    // mprintf("padding   = %u\n", header.bits.padding);
    // mprintf("private   = %u\n", header.bits.private_bit);
    // mprintf("mode      = %u\n", header.bits.mode);
    // mprintf("mode_ext  = %u\n", header.bits.mode_ext);
    // mprintf("copyright = %u\n", header.bits.copyright);
    // mprintf("original  = %u\n", header.bits.original);
    // mprintf("emphasis  = %u\n", header.bits.emphasis);
    // 解析字段
    /*MPEG-1 Layer III*/
    if(header.bits.version==3 && header.bits.layer ==1) 
    {
        ret = (int32_t)(1152.0f/8.0f) * ((float)((mpeg_1_birate_table[header.bits.bitrate])*1000) / (float)(mpeg_1_sample_table[header.bits.sampling]));
        ret += header.bits.padding?1:0;//填充 
    }
    /*MPEG-2 Layer III*/
    else if(header.bits.version==2 && header.bits.layer ==1)
    {
        ret = (int32_t)((576.0f/8.0f) * (float)((mpeg_2_birate_table[header.bits.bitrate])*1000) / (float)(mpeg_2_sample_table[header.bits.sampling]));
        ret += header.bits.padding?1:0;//填充
    }
    // mprintf("ret =%d\r\n",ret);
    return ret;
}
#endif
#if AUDIO_PLAYER_ENABLE
register_audio_decoder(MP3, mp3_decoder_init, mp3_docoder_deinit, get_info, decode_one_frame); 
#endif
#endif
