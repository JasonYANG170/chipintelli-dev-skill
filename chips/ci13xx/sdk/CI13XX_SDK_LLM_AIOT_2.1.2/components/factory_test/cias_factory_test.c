#include "user_config.h"
#if IIS_CHANNEL_ENG_CALC_EANBLE
#include "cias_factory_test.h"
#include "cias_network_msg_protocol.h"
extern CiasAiotFuncParamTypedef gCiasAiotFuncParam;

//生产测试回调函数
void factory_test_play_done_callback(cmd_handle_t cmd_handle)
{
    if(gCiasAiotFuncParam.factory_test_is_busying)
    {
        sys_msg_t send_msg;
		send_msg.msg_type = SYS_MSG_TYPE_FACTORY_TEST_PLAY;
		send_msg_to_sys_task(&send_msg, NULL);
    }
}
// 音频能量检测任务
void cias_audio_eng_check_task(void *p_arg)
{
    uint32_t check_eng_count = 0;
    uint32_t check_eng_pass_count = 0;
    uint8_t send_db_count = 0;
    uint8_t test_rslt = 0;   //测试失败的时间
    gCiasAiotFuncParam.factory_test_is_busying = true;
    // MICL测试
    if (gCiasAiotFuncParam.micl_eng_db_calc_flag)
    {
        check_eng_count = 0;
        check_eng_pass_count = 0;
        while (1)
        {
            if (check_eng_count++ < 30) // 检测30次，超时3S
            {
                if (gCiasAiotFuncParam.upload_factory_test_real_val_flag)
                {
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_REAL_VAL_MICL, &gCiasAiotFuncParam.micl_db, 1, DEF_FILL);
                }
                if (gCiasAiotFuncParam.micl_db > gCiasAiotFuncParam.micl_db_thr_val)
                {
                    check_eng_pass_count++;
                }
                if (check_eng_pass_count > 3) // 测试通过
                {
                    test_rslt = 0x01;
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_MICL, &test_rslt, 1, DEF_FILL);
                    mprintf("MICL通路测试通过==\r\n");
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            else
            {
                break;
            }
        }
        if (test_rslt == 0)
        {
            cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_MICL, &test_rslt, 1, DEF_FILL);
            mprintf("MICL通路测试失败==\r\n");
        }
    }
    test_rslt = 0;
    // MICR测试
    if (gCiasAiotFuncParam.micr_eng_db_calc_flag)
    {
        check_eng_count = 0;
        check_eng_pass_count = 0;
        while (1)
        {
            if (check_eng_count++ < 30) // 检测30次，超时3S
            {
                if (gCiasAiotFuncParam.upload_factory_test_real_val_flag)
                {
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_REAL_VAL_MICR, &gCiasAiotFuncParam.micr_db, 1, DEF_FILL);
                }
                if (gCiasAiotFuncParam.micr_db > gCiasAiotFuncParam.micr_db_thr_val)
                {
                    check_eng_pass_count++;
                }
                if (check_eng_pass_count > 3) // 测试通过
                {
                    test_rslt = 0x01;
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_MICR, &test_rslt, 1, DEF_FILL);
                    mprintf("MICR通路测试通过==\r\n");
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            else
            {
                break;
            }
        }
        if (test_rslt == 0)
        {
            cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_MICR, &test_rslt, 1, DEF_FILL);
            mprintf("MICR通路测试失败==\r\n");
        }
    }
    test_rslt = 0;
    // REFL测试
    if (gCiasAiotFuncParam.refl_eng_db_calc_flag)
    {
        check_eng_count = 0;
        check_eng_pass_count = 0;
        while (1)
        {
            if (check_eng_count++ < 30) // 检测30次，超时3S
            {
                if (gCiasAiotFuncParam.upload_factory_test_real_val_flag)
                {
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_REAL_VAL_REFL, &gCiasAiotFuncParam.refl_db, 1, DEF_FILL);
                }
                if (gCiasAiotFuncParam.refl_db > gCiasAiotFuncParam.refl_db_thr_val)
                {
                    check_eng_pass_count++;
                }
                if (check_eng_pass_count > 3) // 测试通过
                {
                    test_rslt = 0x01;
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_REFL, &test_rslt, 1, DEF_FILL);
                    mprintf("REFL通路测试通过==\r\n");
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            else
            {
                break;
            }
        }
        if (test_rslt == 0)
        {
            cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_REFL, &test_rslt, 1, DEF_FILL);
            mprintf("REFL通路测试失败==\r\n");
        }
    }
    test_rslt = 0;
    // REFR测试
    if (gCiasAiotFuncParam.refr_eng_db_calc_flag)
    {
        check_eng_count = 0;
        check_eng_pass_count = 0;
        while (1)
        {
            if (check_eng_count++ < 30) // 检测30次，超时3S
            {
                if (gCiasAiotFuncParam.upload_factory_test_real_val_flag)
                {
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_REAL_VAL_REFR, &gCiasAiotFuncParam.refr_db, 1, DEF_FILL);
                }
                if (gCiasAiotFuncParam.refr_db > gCiasAiotFuncParam.refr_db_thr_val)
                {
                    check_eng_pass_count++;
                }
                if (check_eng_pass_count > 3) // 测试通过
                {
                    test_rslt = 0x01;
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_REFR, &test_rslt, 1, DEF_FILL);
                    mprintf("REFR通路测试通过==\r\n");
                    break;
                }
                vTaskDelay(pdMS_TO_TICKS(100));
            }
            else
            {
                break;
            }
        }
        if (test_rslt == 0)
        {
            cias_send_cmd_and_data(CIAS_FACTORY_TEST_RSLT_REFR, &test_rslt, 1, DEF_FILL);
            mprintf("REFR通路测试失败==\r\n");
        }
    }
    gCiasAiotFuncParam.factory_test_is_busying = false;
    vTaskDelay(2000);
    vTaskDelete(NULL);
}
bool cias_factory_test_init(void)
{
    if(gCiasAiotFuncParam.factory_test_is_busying)
    {
        return false;
    }
    prompt_play_by_voice_id(FACTORY_TEST_AUDIO_ID, factory_test_play_done_callback, true);  //播放测试音频
    vTaskDelay(pdMS_TO_TICKS(50));
    if (!xTaskCreate(cias_audio_eng_check_task, "cias_audio_eng_check_task", 256, NULL, 4, NULL))
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    return true;
}
#endif