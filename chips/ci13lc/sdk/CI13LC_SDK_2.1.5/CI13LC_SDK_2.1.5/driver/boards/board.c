/**
 * @file board.c
 * @brief 板级配置支持代码
 * @version 1.0.0
 * @date 2021-05-13
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */

#include <string.h>
#include "ci_core_misc.h"
#include "codec_manager.h"
#include "ci_dpmu.h"
#include "ci_gpio.h"
#include "simple_mp3_player.h"
#include "board.h"
#include "ci_assert.h"


/**
* @brief 引脚复用配置为UART功能
*
* @param UARTx UART组 : UART0, UART1
*/
__WEAK void pad_config_for_uart(UART_TypeDef *UARTx)
{
    if (UARTx == UART0)
    {
#if UART0_PIN_EN
        dpmu_set_io_reuse(UART0_TX_PIN, UART0_TX_PIN_REUSE);
	    dpmu_set_io_reuse(UART0_RX_PIN, UART0_RX_PIN_REUSE);
        dpmu_set_io_open_drain(UART0_TX_PIN, UART0_OPEN_DRAIN_EN);
        #if (UART0_OPEN_DRAIN_EN)
        dpmu_set_io_pull(UART0_TX_PIN, DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_pull(UART0_RX_PIN, DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        #else
        dpmu_set_io_pull(UART0_TX_PIN, UART0_TX_PULL);  //RX关闭上拉
        dpmu_set_io_pull(UART0_RX_PIN, UART0_RX_PULL);  //RX需开启上拉
        #endif
#endif
    }
    else if (UARTx == UART1)
    {
#if UART1_PIN_EN
        dpmu_set_io_reuse(UART1_TX_PIN, UART1_TX_PIN_REUSE);
	    dpmu_set_io_reuse(UART1_RX_PIN, UART1_RX_PIN_REUSE);
        dpmu_set_io_open_drain(UART1_TX_PIN, UART1_OPEN_DRAIN_EN);
        #if (UART1_OPEN_DRAIN_EN)
        dpmu_set_io_pull(UART1_TX_PIN, DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_pull(UART1_RX_PIN, DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        #else
        dpmu_set_io_pull(UART1_TX_PIN, UART1_TX_PULL);  //RX关闭上拉
        dpmu_set_io_pull(UART1_RX_PIN, UART1_RX_PULL);  //RX需开启上拉
        #endif
#endif
    }
    else if (UARTx == UART2)
    {
#if UART2_PIN_EN
        dpmu_set_io_reuse(UART2_TX_PIN, UART2_TX_PIN_REUSE);
	    dpmu_set_io_reuse(UART2_RX_PIN, UART2_RX_PIN_REUSE);
        dpmu_set_io_open_drain(UART2_TX_PIN, UART2_OPEN_DRAIN_EN);
        #if (UART2_OPEN_DRAIN_EN)
        dpmu_set_io_pull(UART2_TX_PIN, DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        dpmu_set_io_pull(UART2_RX_PIN, DPMU_IO_PULL_DISABLE);  //RX关闭上拉，使用外部上拉
        #else
        dpmu_set_io_pull(UART2_TX_PIN, UART2_TX_PULL);  //RX关闭上拉
        dpmu_set_io_pull(UART2_RX_PIN, UART2_RX_PULL);  //RX需开启上拉
        #endif
#endif
    }
}

/**
 * @brief 引脚复用配置为IIS功能
 *
 */
__WEAK void pad_config_for_iis(void)
{
#if USE_IIS1_OUT_PRE_RSLT_AUDIO
#if EXT_IIS_PIN_EN
	dpmu_set_io_reuse(EXT_IIS_SDI_PIN         ,EXT_IIS_SDI_PIN_REUSE   );
    dpmu_set_io_reuse(EXT_IIS_LRCLK_PIN       ,EXT_IIS_LRCLK_PIN_REUSE );
    dpmu_set_io_reuse(EXT_IIS_SDO_PIN         ,EXT_IIS_SDO_PIN_REUSE   );
    dpmu_set_io_reuse(EXT_IIS_SCLK_PIN        ,EXT_IIS_SCLK_PIN_REUSE  );
    dpmu_set_io_reuse(EXT_IIS_MCLK_PIN        ,EXT_IIS_MCLK_PIN_REUSE  );
#endif
#endif
}

/**
 * @brief 引脚复用配置为IIC功能
 *
 */
__WEAK void pad_config_for_i2c(void)
{
#if IIC0_PIN_EN
    dpmu_set_io_reuse(IIC0_SDA_PIN, THIRD_FUNCTION);
    dpmu_set_io_reuse(IIC0_SCL_PIN, THIRD_FUNCTION);
#endif
}

int g_pa_pin_valid_level;        //记录功放使能控制引脚的有效电平   1: 高电平，0: 低电平

/**
 * @brief 开启功放使能
 *
 */
__WEAK void power_amplifier_on(void)
{
#if AMP_MUTE_PIN_EN
#if AMP_MUTE_LEVEL_AUTO_DET 
    gpio_set_output_level_single(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN, g_pa_pin_valid_level);
#else
    gpio_set_output_level_single(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN, (1-AMP_MUTE_PIN_MUTE_LEVEL));
#endif
#endif
}

/**
 * @brief 关闭功放使能
 *
 */
__WEAK void power_amplifier_off(void)
{
#if AMP_MUTE_PIN_EN
#if AMP_MUTE_LEVEL_AUTO_DET 
    gpio_set_output_level_single(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN, (1-g_pa_pin_valid_level));
#else
    gpio_set_output_level_single(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN, AMP_MUTE_PIN_MUTE_LEVEL);
#endif
#endif
}

/**
 * @brief 引脚复用配置为GPIO,用于控制功放使能
 *
 */
__WEAK void pad_config_for_power_amplifier(void)
{
#if AMP_MUTE_PIN_EN
    scu_set_device_gate(AMP_MUTE_GPIO_PORT, ENABLE);

    dpmu_set_io_direction(AMP_MUTE_PIN, DPMU_IO_DIRECTION_INPUT);
    dpmu_set_io_pull(AMP_MUTE_PIN, AMP_MUTE_PIN_PULL);
	dpmu_set_io_reuse(AMP_MUTE_PIN, AMP_MUTE_PIN_REUSE);
    gpio_set_input_mode(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN);
    uint8_t pa_default_level = gpio_get_input_level(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN);
    dpmu_set_io_direction(AMP_MUTE_PIN, DPMU_IO_DIRECTION_OUTPUT);
    gpio_set_output_mode(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN);

#if AMP_MUTE_LEVEL_AUTO_DET 
    if(pa_default_level == 0)
    {
       //PA脚如果是下拉的，PA有效电平就是高电平
       g_pa_pin_valid_level = 1;
    }
    else
    {
       g_pa_pin_valid_level = 0;
    }
#else
    gpio_set_output_level_single(AMP_MUTE_GPIO_PORT, AMP_MUTE_GPIO_PIN, AMP_MUTE_PIN_MUTE_LEVEL);
#endif

#if (PLAYER_CONTROL_PA) 
    power_amplifier_off();
#else
    power_amplifier_on();
#endif
#endif
}

/**
 * @brief 1、选择晶振作为时钟源还是RC作为时钟源
 * 
 */
__WEAK void board_clk_source_set(void)
{   
    dpmu_unlock_cfg_config();
    #if USE_EXTERNAL_CRYSTAL_OSC
    dpmu_set_src_source(DPMU_SRC_USE_OUTSIDE_OSC);
    #else
    dpmu_set_src_source(DPMU_SRC_USE_INNER_RC);
    #endif
    dpmu_lock_cfg_config();
}

/******************************默认采音播音的CODEC配置*******************************/
/**
 * @brief 配置参数表
 *
 */
static const cm_codec_hw_info_t host_mic_hw_info = 
{
    #if AUDIO_PLAYER_ENABLE
    .IICx = IIC_NULL,
    .output_iis.IISx = IIS1,
    .output_iis.iis_mode_sel = IIS_MASTER,//IIS_SLAVE,//IIS_MASTER,
    .output_iis.over_sample = IIS_MCLK_FS_256,
    #if !INNER_CODEC_AUDIO_IN_USE_RESAMPLE
    .output_iis.clk_source = IIS_SRC_SOURCE_IPCORE,//AUDIO_PLAY_CLK_SOURCE_OSC_OR_INEER_RC,
    #else
    .output_iis.clk_source = IIS_SRC_SOURCE_IPCORE,//AUDIO_PLAY_CLK_SOURCE_IPCORE,
    #endif
    .output_iis.mclk_out_en = IIS_MCLK_OUT,
    .output_iis.iis_data_format = IIS_DF_IIS,
    .output_iis.sck_lrck_ratio = IIS_SCK_LRCK_64,
    .output_iis.tx_cha = IIS_TX_CHANNAL_TX0,
    .output_iis.scklrck_out_en = IIS_SCKLRCK_MODENULL,
    #endif

    .input_iis.IISx = IIS1,
    .input_iis.iis_mode_sel = IIS_MASTER,
    .input_iis.over_sample = IIS_MCLK_FS_256,
    #if !INNER_CODEC_AUDIO_IN_USE_RESAMPLE
    .input_iis.clk_source = IIS_SRC_SOURCE_IPCORE,//AUDIO_PLAY_CLK_SOURCE_OSC_OR_INEER_RC,
    #else
    .input_iis.clk_source = IIS_SRC_SOURCE_IPCORE,//AUDIO_PLAY_CLK_SOURCE_IPCORE,
    #endif
    .input_iis.mclk_out_en = IIS_MCLK_OUT,
    .input_iis.iis_data_format = IIS_DF_IIS,
    .input_iis.sck_lrck_ratio = IIS_SCK_LRCK_64,
    .input_iis.rx_cha = IIS_RX_CHANNAL_RX0,
    .input_iis.outside_mclk_fre = 0,
    .input_iis.scklrck_out_en = IIS_SCKLRCK_MODENULL,//IIS_SCKLRCK_OUT,

#if (MIC_DIFF_SINGLE == 0)
    .codec_gain.codec_adc_input_mode_l = INNER_CODEC_INPUT_MODE_DIFF,
#else
    .codec_gain.codec_adc_input_mode_l = INNER_CODEC_INPUT_MODE_SINGGLE_ENDED,
#endif
    #if USE_AEC_MODULE
    .codec_gain.codec_adc_input_mode_r = INNER_CODEC_INPUT_MODE_SINGGLE_ENDED,
    #else
    .codec_gain.codec_adc_input_mode_r = INNER_CODEC_INPUT_MODE_DIFF,
    #endif
    .codec_gain.codec_adc_mic_amp_l = INNER_CODEC_MIC_AMP_12dB,
    .codec_gain.codec_adc_mic_amp_r = INNER_CODEC_MIC_AMP_12dB,
    .codec_gain.pga_gain_l = 28.5f,
    .codec_gain.pga_gain_r = 28.5f,
#if (MIC_DIFF_SINGLE == 0)
    .codec_gain.dig_gain_l = 2.0f,//数字增益配置，该值加/减 1 ,数字增益加/减 1dB
    .codec_gain.dig_gain_r = 2.0f,
#else
    .codec_gain.dig_gain_l = 7.0f,//数字增益配置，该值加/减 1 ,数字增益加/减 1dB
    .codec_gain.dig_gain_r = 7.0f,
#endif
    .codec_if = 
    {
        .codec_init = icodec_init,
        .codec_config = icodec_config,
        .codec_start = icodec_start,
        .codec_stop = icodec_stop,
        .codec_ioctl = icodec_ioctl,
    }
}; 

/**
 * @brief 播音配置参数表
 *
 */
static const audio_format_info_t audio_format_info = 
{
    .samprate = 16000,
    .nChans = 2,
    .out_min_size = 1152,
};

/**
 * @brief 录音配置参数表
 *
 */
static const cm_sound_info_t host_mic_sound_info = {
    #if INNER_CODEC_AUDIO_IN_USE_RESAMPLE
    .sample_rate = 32000,
    #else
    .sample_rate = 16000,
    #endif
    #if (2 == HOST_CODEC_CHA_NUM) //使用aec时需要开启双通道 mic和ref通道
    .channel_flag = 3,          //bit[0] = 1选择左声道，bit[1]=1选择右声道，可以用或运算组合
    #else
    .channel_flag = 1,          //bit[0] = 1选择左声道，bit[1]=1选择右声道，可以用或运算组合
    #endif

    .sample_depth = IIS_DW_16BIT,
};

/**
 * @brief 录音codec注册
 *
 */
__WEAK void audio_in_codec_registe()
{
    /*还应该加入codec power up的操作*/
    cm_reg_codec(HOST_MIC_RECORD_CODEC_ID, (cm_codec_hw_info_t*)&host_mic_hw_info);

    uint32_t block_size = 0;
    uint8_t resample_block_double = 1;
    
    #if INNER_CODEC_AUDIO_IN_USE_RESAMPLE
    resample_block_double = 2;
    #endif

    if(3 == host_mic_sound_info.channel_flag)
    {
        block_size = AUDIO_CAP_POINT_NUM_PER_FRM * sizeof(int16_t) * resample_block_double * 2;/*双通道*/
    }
    else
    {
        block_size = AUDIO_CAP_POINT_NUM_PER_FRM * sizeof(int16_t) * resample_block_double;/*双通道*/
    }
    //配置录音PCM buffer
    cm_pcm_buffer_info_t record_buffer_info_str;
    record_buffer_info_str.record_buffer_info.block_num = AUDIO_IN_BUFFER_NUM;
    record_buffer_info_str.record_buffer_info.block_size = block_size;//576;
    record_buffer_info_str.record_buffer_info.buffer_size = record_buffer_info_str.record_buffer_info.block_size * record_buffer_info_str.record_buffer_info.block_num;
    record_buffer_info_str.record_buffer_info.pcm_buffer = (void*)pvPortMalloc(record_buffer_info_str.record_buffer_info.buffer_size);
    cm_config_pcm_buffer(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT, &record_buffer_info_str);
    //配置录音音频格式
    cm_config_codec(HOST_MIC_RECORD_CODEC_ID, CODEC_INPUT, (cm_sound_info_t*)&host_mic_sound_info);

    #if 0/*内部ADC探针模式*/
    scu_iis_pad_data_config(PAD_IIS_DATA_FROM_CODEC_ADC);
    #endif

    // #if ((CI_CHIP_TYPE == 13160) || (CI_CHIP_TYPE == 13161) || (CI_CHIP_TYPE == 13162))
    if (strncmp(TOSTRING(CI_CHIP_TYPE), "1316",4) == 0)
    {
        #if INNER_AUDIO_PA
        //内置功放的芯片
        dpmu_set_io_pull(20,DPMU_IO_PULL_DISABLE);
        #else
        //ssop16的vcm使用外部的，pc4需关闭上下拉，codec配置vcm电压
        dpmu_set_io_pull(29,DPMU_IO_PULL_DISABLE);
        inner_codec_vcm_avdd(INNER_CODEC_VCM_VOL_0_533);
        #endif
    }

    // #if ((CI_CHIP_TYPE == 13080) || (CI_CHIP_TYPE == 13081) || (CI_CHIP_TYPE == 13082))
    if (strncmp(TOSTRING(CI_CHIP_TYPE), "1308",4) == 0)
    {
        //ssop8的vcm使用内部的，pc0需关闭上下拉
        dpmu_set_io_pull(20,DPMU_IO_PULL_DISABLE);
        //ssop8的新封装，mic和pc1接一起需关闭上下拉
        dpmu_set_io_pull(26,DPMU_IO_PULL_DISABLE);
    }
    
    pad_config_for_power_amplifier();
}   


/*******************************语音前处理输出的CODEC配置******************************/
/**
 * @brief 语音前处理codec配置参数表
 *
 */
static const cm_codec_hw_info_t audio_pre_tslt_out_info = 
{
    .IICx = IIC_NULL,
    .output_iis.IISx = IIS0,
    .output_iis.iis_mode_sel = IIS_MASTER,
    .output_iis.over_sample = IIS_MCLK_FS_256,
    #if !INNER_CODEC_AUDIO_IN_USE_RESAMPLE
    .output_iis.clk_source = AUDIO_PLAY_CLK_SOURCE_IPCORE,//AUDIO_PLAY_CLK_SOURCE_OSC_OR_INEER_RC,
    #else
    .output_iis.clk_source = AUDIO_PLAY_CLK_SOURCE_IPCORE,//AUDIO_PLAY_CLK_SOURCE_IPCORE,
    #endif
    .output_iis.mclk_out_en = IIS_MCLK_OUT,
    .output_iis.iis_data_format = IIS_DF_IIS,
    .output_iis.sck_lrck_ratio = IIS_SCK_LRCK_64,
    .output_iis.tx_cha = IIS_TX_CHANNAL_TX0,
    .output_iis.scklrck_out_en = IIS_SCKLRCK_OUT,
}; 


    /**
 * @brief 语音前处理播音配置参数表
 *
 */
static const cm_sound_info_t pre_rslt_out_sound_info = 
{
    .sample_rate = 16000,
    .channel_flag = 3,
    .sample_depth = IIS_DW_16BIT,
};


/**
 * @brief 语音前处理使用CODEC的初始化
 * 
 */
__WEAK void audio_pre_rslt_out_codec_init(void)
{
    uint16_t block_size = AUDIO_CAP_POINT_NUM_PER_FRM * 2 * sizeof(int16_t);

    cm_reg_codec(PLAY_PRE_AUDIO_CODEC_ID, (cm_codec_hw_info_t*)&audio_pre_tslt_out_info);

    cm_pcm_buffer_info_t pcm_buffer_info;
    pcm_buffer_info.play_buffer_info.block_num = 1;
    pcm_buffer_info.play_buffer_info.buffer_num = 6;
    pcm_buffer_info.play_buffer_info.block_size = block_size;
    pcm_buffer_info.play_buffer_info.buffer_size = pcm_buffer_info.play_buffer_info.block_size * pcm_buffer_info.play_buffer_info.block_num;
    pcm_buffer_info.play_buffer_info.pcm_buffer = pvPortMalloc(pcm_buffer_info.play_buffer_info.buffer_size * pcm_buffer_info.play_buffer_info.buffer_num);
    CI_ASSERT(pcm_buffer_info.play_buffer_info.pcm_buffer,"\n");
    cm_config_pcm_buffer(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT, &pcm_buffer_info);
    //配置IIS放音音频格式
    
    cm_config_codec(PLAY_PRE_AUDIO_CODEC_ID, CODEC_OUTPUT, (cm_sound_info_t*)&pre_rslt_out_sound_info);
}

