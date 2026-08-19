#include "cias_factory_test.h"
#include "cias_network_msg_protocol.h"
extern CiasAiotFuncParamTypedef gCiasAiotFuncParam;

// 音频能量检测任务
void cias_audio_eng_check_task(void *p_arg)
{
    uint32_t check_eng_count = 0;
    uint32_t check_eng_pass_count = 0;
    uint8_t  send_db_count = 0;
    ciss_set(CI_SS_ALG_CALC_AUDIO_ENG_ENABLE, 1); // 打开能量值计算
    while (1)
    {
        if (check_eng_count++ < 30) // 检测30次，超时3S
        {
            if (gCiasAiotFuncParam.upload_factory_test_real_val_flag)
            {
                if(send_db_count++ >= 10)
                {
                    send_db_count = 0;
                    uint8_t real_db = ciss_get(CI_SS_ALG_AUDIO_ENG_VAL);
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_REAL_VAL_GET, &real_db, 1, DEF_FILL);
                }
            }
            if (ciss_get(CI_SS_ALG_AUDIO_ENG_VAL) > gCiasAiotFuncParam.is_have_audio_eng_val)
            {
                check_eng_pass_count++;
            }
            if (check_eng_pass_count > 3) // 测试通过
            {
                if (gCiasAiotFuncParam.upload_factory_test_real_val_flag)
                {
                    send_db_count = 0;
                    uint8_t real_db = ciss_get(CI_SS_ALG_AUDIO_ENG_VAL);
                    cias_send_cmd_and_data(CIAS_FACTORY_TEST_REAL_VAL_GET, &real_db, 1, DEF_FILL);
                }
                uint8_t test_result = 0x01;
                cias_send_cmd_and_data(CIAS_FACTORY_TEST_ENG_GET, &test_result, 1, DEF_FILL);
                mprintf("音频通路测试通过==\r\n");
                vTaskDelay(2000);
                vTaskDelete(NULL);
           }
        }
        else // 测试失败
        {
            mprintf("音频通路测试失败==\r\n");
            uint8_t test_result = 0x02;
            cias_send_cmd_and_data(CIAS_FACTORY_TEST_ENG_GET, &test_result, 1, DEF_FILL);
            ciss_set(CI_SS_ALG_CALC_AUDIO_ENG_ENABLE, 0); // 关闭能量值计算
            vTaskDelay(2000);
            vTaskDelete(NULL);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
bool cias_factory_test_init(void)
{
    if (!xTaskCreate(cias_audio_eng_check_task, "cias_audio_eng_check_task", 256, NULL, 4, NULL))
    {
        mprintf("error %s  %d\n", __func__, __LINE__);
        return false;
    }
    return true;
}