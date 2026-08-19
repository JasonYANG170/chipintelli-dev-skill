#include "cias_ir_flash.h"
#include "cias_log.h"
#include "cias_freertos_common.h"
#include "cias_common.h"
#include "tuya_error_code.h"
#define FALSH_SIZE_4K (1024 * 4)
void cias_ln_flash_write(uint32_t offset, uint32_t length, uint8_t *buffer)
{
    uint32_t erase_offset = offset;
    uint32_t offset_reamin = 0;

    if (offset % FALSH_SIZE_4K != 0)
    {
        erase_offset = (offset / FALSH_SIZE_4K) * FALSH_SIZE_4K; //擦除地址4K对齐
        offset_reamin = offset % FALSH_SIZE_4K;
    }
    CIAS_LOG_HL("erase_offset = 0x%x, offset_reamin = %d, length = %d", erase_offset, offset_reamin, length);
    uint8_t *temp_buf = cias_malloc(FALSH_SIZE_4K);
    if (temp_buf)
    {
        hal_flash_read_by_cache(erase_offset, FALSH_SIZE_4K + offset_reamin, temp_buf);
        CIAS_LOG_HL("---hal_flash_erase start ...");
        hal_flash_erase(erase_offset, FALSH_SIZE_4K);
        CIAS_LOG_HL("---hal_flash_erase end ...");
        for (int i = 0; i < length; i++)
            temp_buf[offset_reamin + i] = buffer[i];
        CIAS_LOG_HL("---hal_flash_program end ...");
        hal_flash_program(erase_offset, FALSH_SIZE_4K, temp_buf);
        CIAS_LOG_HL("---hal_flash_program end ...");
        cias_free(temp_buf);
        temp_buf = NULL;
        if ((offset + length) > (erase_offset + FALSH_SIZE_4K))
        {
            CIAS_LOG_HL("offset + length - (erase_offset + FALSH_SIZE_4K) = %d", offset + length - (erase_offset + FALSH_SIZE_4K));
            temp_buf = cias_malloc(FALSH_SIZE_4K);
            if (temp_buf)
            {
                CIAS_LOG_HL("----second write...");
                hal_flash_read_by_cache(erase_offset + FALSH_SIZE_4K, FALSH_SIZE_4K, temp_buf);
                hal_flash_erase(erase_offset + FALSH_SIZE_4K, FALSH_SIZE_4K);
                for (int i = 0; i < (offset + length - (erase_offset + FALSH_SIZE_4K)); i++)
                {
                    temp_buf[i] = buffer[erase_offset + FALSH_SIZE_4K + i];
                }
                hal_flash_program(erase_offset + FALSH_SIZE_4K, FALSH_SIZE_4K, temp_buf);
                CIAS_LOG_HL("----second write ok...");
                cias_free(temp_buf);
            }
        }
    }
}
void cias_ir_partition_erase(int tuya_ir_dev_type)
{
    uint32_t erase_offset = 0;
    switch (tuya_ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
        CIAS_LOG_HL("机顶盒设备");
        erase_offset = IR_SET_TOP_BOX_START_ADDR;
        break;
    case IR_IV_DATA_ID:
        CIAS_LOG_HL("电视设备");
        erase_offset = IR_IV_DATA_START_ADDR;
        break;
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("电视盒子设备");
        erase_offset = IR_TV_BOX_START_ADDR;
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备");
        erase_offset = IR_DVD_START_ADDR;
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备");
        erase_offset = IR_AIR_CONDITIONER_START_ADDR;
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备");
        erase_offset = IR_PROJECTOR_START_ADDR;
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备");
        erase_offset = IR_AUDIO_START_ADDR;
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备");
        erase_offset = IR_FAN_START_ADDR;
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备");
        erase_offset = IR_CAMERA_START_ADDR;
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备");
        erase_offset = IR_LIGHT_START_ADDR;
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备");
        erase_offset = IR_FILTER_START_ADDR;
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备");
        erase_offset = IR_GEYSER_START_ADDR;
        break;
    case IR_DIY_ID:
        CIAS_LOG_HL("DIY设备");
        erase_offset = IR_DIY_START_ADDR;
        break;
    default:
        CIAS_LOG_HL("cias_ir_partition_erase: param error");
        return;
    }
    if (erase_offset != 0)
        hal_flash_erase(erase_offset, IR_STORE_DATA_MAX_SIZE);
}

//红外数据写入
void cias_ir_store_data_write(int offset, int wlen, uint8_t *wdata)
{
    CIAS_LOG_HL("cias_ir_store_data_write:offset = 0x%x, wlen = %d， wdata = %s", offset, wlen, wdata);
    hal_flash_program(offset, wlen, wdata);
}
//红外数据写入
void cias_ir_store_data_read(int offset, int rlen, uint8_t *rdata)
{
    hal_flash_read_by_cache(offset, rlen, rdata);
    CIAS_LOG_HL("cias_ir_store_data_read:offset = 0x%x, rlen = %d, rdata = %s", offset, rlen, rdata);
}
//涂鸦红外数据存储头读取
int tuya_ir_store_head_info_read(uint8_t tuya_ir_dev_type, tuya_ir_code_head_t *ptuya_ir_code_head)
{
    uint32_t read_offset = 0;
    CIAS_LOG_HL("tuya_ir_store_head_info_read is called....");
    switch (tuya_ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
        CIAS_LOG_HL("机顶盒设备");
        read_offset = IR_SET_TOP_BOX_START_ADDR;
        break;
    case IR_IV_DATA_ID:
        CIAS_LOG_HL("电视设备");
        read_offset = IR_IV_DATA_START_ADDR;
        break;
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("电视盒子设备");
        read_offset = IR_TV_BOX_START_ADDR;
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备");
        read_offset = IR_DVD_START_ADDR;
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备");
        read_offset = IR_AIR_CONDITIONER_START_ADDR;
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备");
        read_offset = IR_PROJECTOR_START_ADDR;
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备");
        read_offset = IR_AUDIO_START_ADDR;
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备");
        read_offset = IR_FAN_START_ADDR;
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备");
        read_offset = IR_CAMERA_START_ADDR;
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备");
        read_offset = IR_LIGHT_START_ADDR;
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备");
        read_offset = IR_FILTER_START_ADDR;
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备");
        read_offset = IR_GEYSER_START_ADDR;
        break;
    default:
        CIAS_LOG_HL("tuya_ir_store_head_info_write: param error");
        return;
    }
    hal_flash_read_by_cache(read_offset, sizeof(tuya_ir_code_head_t) - 8, (uint8_t *)ptuya_ir_code_head);
    CIAS_LOG_HL("ptuya_ir_code_head->ir_available_flag = 0x%x", ptuya_ir_code_head->ir_available_flag);
    CIAS_LOG_HL("ptuya_ir_code_head->ir_remote_id = %d", ptuya_ir_code_head->ir_remote_id);
    CIAS_LOG_HL("ptuya_ir_code_head->tuya_ir_dev_type = %d", ptuya_ir_code_head->tuya_ir_dev_type);
    CIAS_LOG_HL("ptuya_ir_code_head->ir_data_num = %d", ptuya_ir_code_head->ir_data_num);
    CIAS_LOG_HL("ptuya_ir_code_head->ir_dev_id_len = %d", ptuya_ir_code_head->ir_dev_id_len);
    CIAS_LOG_HL("ptuya_ir_code_head->ir_head_id_len = %d", ptuya_ir_code_head->ir_head_id_len);
    CIAS_LOG_HL("ptuya_ir_code_head->ir_available_flag = %d", ptuya_ir_code_head->ir_available_flag);
    if (ptuya_ir_code_head->ir_available_flag == 0x5A)
    {
        if (ptuya_ir_code_head->ir_dev_id_len > 0)
        {
            ptuya_ir_code_head->ir_dev_id = cias_malloc(ptuya_ir_code_head->ir_dev_id_len);
            if (ptuya_ir_code_head->ir_dev_id)
            {
                hal_flash_read_by_cache((read_offset + sizeof(tuya_ir_code_head_t) - 8), ptuya_ir_code_head->ir_dev_id_len, ptuya_ir_code_head->ir_dev_id);
                CIAS_LOG_HL("ptuya_ir_code_head->ir_dev_id = %s", ptuya_ir_code_head->ir_dev_id);
            }
            else
            {
                CIAS_LOG_HL("tuya_ir_store_head_info_read:ptuya_ir_code_head->ir_dev_id malloc error..");
            }
        }
        if (ptuya_ir_code_head->ir_head_id_len > 0)
        {
            ptuya_ir_code_head->ir_head_id = cias_malloc(ptuya_ir_code_head->ir_head_id_len);
            if (ptuya_ir_code_head->ir_head_id)
            {
                hal_flash_read_by_cache((read_offset + sizeof(tuya_ir_code_head_t) - 8) + ptuya_ir_code_head->ir_dev_id_len, ptuya_ir_code_head->ir_head_id_len, ptuya_ir_code_head->ir_head_id);
                CIAS_LOG_HL("ptuya_ir_code_head->ir_head_id = %s", ptuya_ir_code_head->ir_head_id);
            }
            else
            {
                CIAS_LOG_HL("tuya_ir_store_head_info_read:ptuya_ir_code_head->ir_head_id malloc error..");
            }
        }
        return 0;
    }
    return -1;
}
int check_ir_data_available(uint8_t tuya_ir_dev_type, tuya_ir_code_head_t *ptuya_ir_code_head)
{
    uint32_t read_offset = 0;
    CIAS_LOG_HL("check_ir_data_available is called....");
    switch (tuya_ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
        CIAS_LOG_HL("机顶盒设备");
        read_offset = IR_SET_TOP_BOX_START_ADDR;
        break;
    case IR_IV_DATA_ID:
        CIAS_LOG_HL("电视设备");
        read_offset = IR_IV_DATA_START_ADDR;
        break;
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("电视盒子设备");
        read_offset = IR_TV_BOX_START_ADDR;
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备");
        read_offset = IR_DVD_START_ADDR;
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备");
        read_offset = IR_AIR_CONDITIONER_START_ADDR;
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备");
        read_offset = IR_PROJECTOR_START_ADDR;
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备");
        read_offset = IR_AUDIO_START_ADDR;
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备");
        read_offset = IR_FAN_START_ADDR;
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备");
        read_offset = IR_CAMERA_START_ADDR;
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备");
        read_offset = IR_LIGHT_START_ADDR;
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备");
        read_offset = IR_FILTER_START_ADDR;
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备");
        read_offset = IR_GEYSER_START_ADDR;
        break;
    default:
        CIAS_LOG_HL("tuya_ir_store_head_info_write: param error");
        return;
    }
    hal_flash_read_by_cache(read_offset, sizeof(tuya_ir_code_head_t) - 8, (uint8_t *)ptuya_ir_code_head);
    CIAS_LOG_HL("ptuya_ir_code_head->ir_available_flag = 0x%x", ptuya_ir_code_head->ir_available_flag);
    if(ptuya_ir_code_head->ir_available_flag == 0x5A)
    {
        return 0;
    }
    return -1;
}
//涂鸦红外数据存储头写入
void tuya_ir_store_head_info_write(uint8_t tuya_ir_dev_type, const tuya_ir_code_head_t tuya_ir_code_head)
{
    uint32_t write_offset = 0;
    CIAS_LOG_HL("tuya_ir_store_head_info_write is called...., tuya_ir_code_head.ir_available_flag = 0x%x", tuya_ir_code_head.ir_available_flag);
    switch (tuya_ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
        CIAS_LOG_HL("机顶盒设备");
        write_offset = IR_SET_TOP_BOX_START_ADDR;
        break;
    case IR_IV_DATA_ID:
        CIAS_LOG_HL("电视设备");
        write_offset = IR_IV_DATA_START_ADDR;
        break;
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("电视盒子设备");
        write_offset = IR_TV_BOX_START_ADDR;
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备");
        write_offset = IR_DVD_START_ADDR;
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备");
        write_offset = IR_AIR_CONDITIONER_START_ADDR;
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备");
        write_offset = IR_PROJECTOR_START_ADDR;
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备");
        write_offset = IR_AUDIO_START_ADDR;
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备");
        write_offset = IR_FAN_START_ADDR;
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备");
        write_offset = IR_CAMERA_START_ADDR;
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备");
        write_offset = IR_LIGHT_START_ADDR;
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备");
        write_offset = IR_FILTER_START_ADDR;
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备");
        write_offset = IR_GEYSER_START_ADDR;
        break;
    default:
        CIAS_LOG_HL("tuya_ir_store_head_info_write: param error");
        return;
    }
    int m_write_len = sizeof(tuya_ir_code_head_t) - 8 + tuya_ir_code_head.ir_dev_id_len + tuya_ir_code_head.ir_head_id_len;
    uint8_t m_write_buf[m_write_len];
    memcpy(m_write_buf, &tuya_ir_code_head, sizeof(tuya_ir_code_head_t) - 8);
    if (tuya_ir_code_head.ir_dev_id != NULL)
    {
        CIAS_LOG_HL("tuya_ir_code_head.ir_dev_id = %s", tuya_ir_code_head.ir_dev_id);
        memcpy(m_write_buf + sizeof(tuya_ir_code_head_t) - 8, tuya_ir_code_head.ir_dev_id, tuya_ir_code_head.ir_dev_id_len);
    }
    if (tuya_ir_code_head.ir_head_id != NULL)
    {
        CIAS_LOG_HL("tuya_ir_code_head.ir_head_id = %s", tuya_ir_code_head.ir_head_id);
        memcpy(m_write_buf + sizeof(tuya_ir_code_head_t) - 8 + tuya_ir_code_head.ir_dev_id_len, tuya_ir_code_head.ir_head_id, tuya_ir_code_head.ir_head_id_len);
    }
    cias_ir_store_data_write(write_offset, m_write_len, m_write_buf);
    CIAS_LOG_HL("m_write_buf = %s", m_write_buf + sizeof(tuya_ir_code_head_t) - 8);
    CIAS_LOG_HL("m_write_len = %d", m_write_len);
    CIAS_LOG_HL("tuya_ir_code_head.ir_remote_id = %d", tuya_ir_code_head.ir_remote_id);
    CIAS_LOG_HL("tuya_ir_code_head.tuya_ir_dev_type = %d", tuya_ir_code_head.tuya_ir_dev_type);
    CIAS_LOG_HL("tuya_ir_code_head.ir_data_num = %d", tuya_ir_code_head.ir_data_num);
    CIAS_LOG_HL("tuya_ir_code_head.ir_dev_id_len = %d", tuya_ir_code_head.ir_dev_id_len);
    CIAS_LOG_HL("tuya_ir_code_head.ir_head_id_len = %d", tuya_ir_code_head.ir_head_id_len);
    if (tuya_ir_code_head.ir_dev_id_len > 0)
        CIAS_LOG_HL("tuya_ir_code_head.ir_dev_id = %s", tuya_ir_code_head.ir_dev_id);
    if (tuya_ir_code_head.ir_head_id_len)
        CIAS_LOG_HL("tuya_ir_code_head.ir_head_id = %s", tuya_ir_code_head.ir_head_id);
}

void tuya_ir_data_write_flash(uint8_t tuya_ir_dev_type, uint32_t offset, uint8_t *ir_data_buf, int ir_data_len)
{
    CIAS_LOG_HL("tuya_ir_data_write_flash is called....");
    CIAS_LOG_HL("tuya_ir_dev_type = %d, offset = 0x%x", tuya_ir_dev_type, offset);
    uint32_t write_offset = offset;
    switch (tuya_ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
        CIAS_LOG_HL("机顶盒设备");
        write_offset += IR_SET_TOP_BOX_START_ADDR;
        break;
    case IR_IV_DATA_ID:
        CIAS_LOG_HL("电视设备");
        write_offset += IR_IV_DATA_START_ADDR;
        break;
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("电视盒子设备");
        write_offset += IR_TV_BOX_START_ADDR;
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备");
        write_offset += IR_DVD_START_ADDR;
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备");
        write_offset += IR_AIR_CONDITIONER_START_ADDR;
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备");
        write_offset += IR_PROJECTOR_START_ADDR;
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备");
        write_offset += IR_AUDIO_START_ADDR;
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备");
        write_offset += IR_FAN_START_ADDR;
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备");
        write_offset += IR_CAMERA_START_ADDR;
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备");
        write_offset += IR_LIGHT_START_ADDR;
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备");
        write_offset += IR_FILTER_START_ADDR;
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备");
        write_offset += IR_GEYSER_START_ADDR;
        break;
    default:
        CIAS_LOG_HL("tuya_ir_data_write_flash: param error");
        return;
    }
    cias_ir_store_data_write(write_offset, ir_data_len, ir_data_buf);
}

void tuya_ir_data_read_from_flash(uint8_t tuya_ir_dev_type)
{
    //读红外数据存储头
    tuya_ir_code_head_t tuya_ir_code_head;
    uint32_t read_offset = 0x0;
    CIAS_LOG_HL("tuya_ir_data_read_from_flash is called....");
    switch (tuya_ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
        CIAS_LOG_HL("机顶盒设备");
        read_offset = IR_SET_TOP_BOX_START_ADDR;
        break;
    case IR_IV_DATA_ID:
        CIAS_LOG_HL("电视设备");
        read_offset = IR_IV_DATA_START_ADDR;
        break;
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("电视盒子设备");
        read_offset = IR_TV_BOX_START_ADDR;
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备");
        read_offset = IR_DVD_START_ADDR;
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备");
        read_offset = IR_AIR_CONDITIONER_START_ADDR;
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备");
        read_offset = IR_PROJECTOR_START_ADDR;
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备");
        read_offset = IR_AUDIO_START_ADDR;
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备");
        read_offset = IR_FAN_START_ADDR;
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备");
        read_offset = IR_CAMERA_START_ADDR;
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备");
        read_offset = IR_LIGHT_START_ADDR;
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备");
        read_offset = IR_FILTER_START_ADDR;
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备");
        read_offset = IR_GEYSER_START_ADDR;
        break;
    default:
        CIAS_LOG_HL("cias_read_ir_data_from_flash: param error");
        return;
    }
    CIAS_LOG_HL("tuya_ir_data_read_from_flash:offset = 0x%x", read_offset);
    tuya_ir_store_head_info_read(tuya_ir_dev_type, &tuya_ir_code_head);
    int ir_temp_offset = read_offset + (sizeof(tuya_ir_code_head_t) - 8) + tuya_ir_code_head.ir_dev_id_len + tuya_ir_code_head.ir_head_id_len;
    CIAS_LOG_HL("tuya_ir_code_head.ir_available_flag = %d", tuya_ir_code_head.ir_available_flag);
    CIAS_LOG_HL("tuya_ir_code_head.ir_remote_id = %d", tuya_ir_code_head.ir_remote_id);
    CIAS_LOG_HL("tuya_ir_code_head.tuya_ir_dev_type = %d", tuya_ir_code_head.tuya_ir_dev_type);
    CIAS_LOG_HL("tuya_ir_code_head.ir_dev_id_len = %d", tuya_ir_code_head.ir_dev_id_len);
    CIAS_LOG_HL("tuya_ir_code_head.ir_head_id_len = %d", tuya_ir_code_head.ir_head_id_len);
    CIAS_LOG_HL("--ir_temp_offset = 0x%x", ir_temp_offset);
    CIAS_LOG_HL("tuya_ir_code_head.ir_data_num = %d", tuya_ir_code_head.ir_data_num);
    if (tuya_ir_code_head.ir_available_flag != 0x5A)
    {
        CIAS_LOG_ERR("------the ir is not available...\r\n");
        return;
    }
    if (tuya_ir_dev_type == IR_AIR_CONDITIONER_ID)
    {
        //读空调设备红外存储具体数据
        tuya_conditioner_ir_code_data_t tuya_ir_code_data;
        for (int i = 0; i < tuya_ir_code_head.ir_data_num; i++)
        {
            //CIAS_LOG_HL("tuya_ir_data_read_from_flash: tuya_ir_code_head.ir_data_num = %d, ir_temp_offset = 0x%x, cur_num = %d", tuya_ir_code_head.ir_data_num, ir_temp_offset, i);
            hal_flash_read_by_cache(ir_temp_offset, sizeof(tuya_conditioner_ir_code_data_t) - 8, (uint8_t *)&tuya_ir_code_data);
            //CIAS_LOG_HL("tuya_ir_code_data.ir_key_len = %d", tuya_ir_code_data.ir_key_len);
            // CIAS_LOG_HL("tuya_ir_code_data.ir_data_len = %d", tuya_ir_code_data.ir_data_len);
            ir_temp_offset += sizeof(tuya_conditioner_ir_code_data_t) - 8;
            if (tuya_ir_code_data.ir_key_len > 0)
            {
                tuya_ir_code_data.ir_key = cias_malloc(tuya_ir_code_data.ir_key_len);
                if (tuya_ir_code_data.ir_key > 0) //读红外数据key
                {
                    memset(tuya_ir_code_data.ir_key, 0, tuya_ir_code_data.ir_key_len);
                    hal_flash_read_by_cache(ir_temp_offset, tuya_ir_code_data.ir_key_len, tuya_ir_code_data.ir_key);
                    log_printf("tuya_ir_code_data.ir_key = %s, ", tuya_ir_code_data.ir_key);
                }
                else
                {
                    CIAS_LOG_HL("tuya_ir_data_read_from_flash:tuya_ir_code_data.ir_key malloc error");
                }
                ir_temp_offset += tuya_ir_code_data.ir_key_len;
                if (tuya_ir_code_data.ir_key)
                    free(tuya_ir_code_data.ir_key);
            }
            if (tuya_ir_code_data.ir_data_len > 0)
            {
                tuya_ir_code_data.ir_data = cias_malloc(tuya_ir_code_data.ir_data_len);
                if (tuya_ir_code_data.ir_data) //读红外数据code
                {
                    memset(tuya_ir_code_data.ir_data, 0, tuya_ir_code_data.ir_data_len);
                    hal_flash_read_by_cache(ir_temp_offset, tuya_ir_code_data.ir_data_len, tuya_ir_code_data.ir_data);
                    CIAS_LOG_HL("tuya_ir_code_head.ir_data = %s", tuya_ir_code_data.ir_data);
                }
                else
                {
                    CIAS_LOG_HL("1tuya_ir_data_read_from_flash:tuya_ir_code_data.ir_data malloc error");
                }
                ir_temp_offset += tuya_ir_code_data.ir_data_len;
                if (tuya_ir_code_data.ir_data)
                    free(tuya_ir_code_data.ir_data);
            }
        }
    }
    else
    {
        //读通用设备红外存储具体数据
        tuya_universal_ir_code_data_t tuya_ir_code_data;
        for (int i = 0; i < tuya_ir_code_head.ir_data_num; i++)
        {
            memset(&tuya_ir_code_data, 0, sizeof(tuya_conditioner_ir_code_data_t));
            CIAS_LOG_HL("tuya_ir_data_read_from_flash: tuya_ir_code_head.ir_data_num = %d, ir_temp_offset = 0x%x, cur_num = %d", tuya_ir_code_head.ir_data_num, ir_temp_offset, i);
            hal_flash_read_by_cache(ir_temp_offset, sizeof(tuya_universal_ir_code_data_t) - 4, (uint8_t *)&tuya_ir_code_data);
            CIAS_LOG_HL("tuya_ir_code_data.keyid = %d", tuya_ir_code_data.keyid);
            CIAS_LOG_HL("tuya_ir_code_data.ir_data_len = %d", tuya_ir_code_data.ir_data_len);
            ir_temp_offset += sizeof(tuya_universal_ir_code_data_t) - 4;
            if ((tuya_ir_code_data.ir_data_len > 0) && (tuya_ir_code_data.keyid > 0))
            {
                tuya_ir_code_data.ir_data = cias_malloc(tuya_ir_code_data.ir_data_len);
                if (tuya_ir_code_data.ir_data) //读红外数据code
                {
                    memset(tuya_ir_code_data.ir_data, 0, tuya_ir_code_data.ir_data_len);
                    hal_flash_read_by_cache(ir_temp_offset, tuya_ir_code_data.ir_data_len, tuya_ir_code_data.ir_data);
                    log_printf("tuya_ir_code_data.keyid = %s, ", tuya_ir_code_data.keyid);
                    CIAS_LOG_HL("tuya_ir_code_head.ir_data = %s", tuya_ir_code_data.ir_data);
                }
                else
                {
                    CIAS_LOG_HL("2tuya_ir_data_read_from_flash:tuya_ir_code_data.ir_data malloc error");
                }
                ir_temp_offset += tuya_ir_code_data.ir_data_len;
                if (tuya_ir_code_data.ir_data)
                    cias_free(tuya_ir_code_data.ir_data);
            }
        }
    }

    CIAS_LOG_HL("tuya_ir_code_head.ir_dev_id free...\r\n");
    if (tuya_ir_code_head.ir_dev_id)
        cias_free(tuya_ir_code_head.ir_dev_id);
    if (tuya_ir_code_head.ir_head_id)
        cias_free(tuya_ir_code_head.ir_head_id);
    CIAS_LOG_HL("tuya_ir_code_head.ir_dev_id end...\r\n");
}
/*
 * 根据ir_key/ir_key_value从flash中查找对应的红外码库
 * tuya_ir_dev_type:红外设备类型
 * ir_key:空调设备的key
 * ir_key_value:非空调设备的key id
 */
int tuya_ir_data_find_from_flash(uint8_t tuya_ir_dev_type, uint8_t *ir_key, int ir_key_value)
{
    //读红外数据存储头
    tuya_ir_code_head_t tuya_ir_code_head;
    uint32_t read_offset = 0x0;
    int find_key_flag = 1;
    int ret = OPRT_OK;
    CIAS_LOG_HL("tuya_ir_data_find_from_flash is called....");
    switch (tuya_ir_dev_type)
    {
    case IR_SET_TOP_BOX_ID:
        CIAS_LOG_HL("机顶盒设备");
        read_offset = IR_SET_TOP_BOX_START_ADDR;
        break;
    case IR_IV_DATA_ID:
        CIAS_LOG_HL("电视设备");
        read_offset = IR_IV_DATA_START_ADDR;
        break;
    case IR_TV_BOX_ID:
        CIAS_LOG_HL("电视盒子设备");
        read_offset = IR_TV_BOX_START_ADDR;
        break;
    case IR_DVD_ID:
        CIAS_LOG_HL("DVD设备");
        read_offset = IR_DVD_START_ADDR;
        break;
    case IR_AIR_CONDITIONER_ID:
        CIAS_LOG_HL("空调设备");
        read_offset = IR_AIR_CONDITIONER_START_ADDR;
        break;
    case IR_PROJECTOR_ID:
        CIAS_LOG_HL("投影仪设备");
        read_offset = IR_PROJECTOR_START_ADDR;
        break;
    case IR_AUDIO_ID:
        CIAS_LOG_HL("音响设备");
        read_offset = IR_AUDIO_START_ADDR;
        break;
    case IR_FAN_ID:
        CIAS_LOG_HL("风扇设备");
        read_offset = IR_FAN_START_ADDR;
        break;
    case IR_CAMERA_ID:
        CIAS_LOG_HL("单反相机设备");
        read_offset = IR_CAMERA_START_ADDR;
        break;
    case IR_LIGHT_ID:
        CIAS_LOG_HL("灯光设备");
        read_offset = IR_LIGHT_START_ADDR;
        break;
    case IR_FILTER_ID:
        CIAS_LOG_HL("净化器设备");
        read_offset = IR_FILTER_START_ADDR;
        break;
    case IR_GEYSER_ID:
        CIAS_LOG_HL("热水器设备");
        read_offset = IR_GEYSER_START_ADDR;
        break;
    default:
        CIAS_LOG_HL("cias_read_ir_data_from_flash: param error");
        return OPRT_RECV_ERR;
    }
    CIAS_LOG_HL("tuya_ir_data_read_from_flash:offset = 0x%x", read_offset);
    tuya_ir_store_head_info_read(tuya_ir_dev_type, &tuya_ir_code_head);
    int ir_temp_offset = read_offset + (sizeof(tuya_ir_code_head_t) - 8) + tuya_ir_code_head.ir_dev_id_len + tuya_ir_code_head.ir_head_id_len;
    // CIAS_LOG_HL("tuya_ir_code_head.ir_available_flag = %d", tuya_ir_code_head.ir_available_flag);
    // CIAS_LOG_HL("tuya_ir_code_head.ir_dev_id_len = %d", tuya_ir_code_head.ir_dev_id_len);
    // CIAS_LOG_HL("tuya_ir_code_head.ir_head_id_len = %d", tuya_ir_code_head.ir_head_id_len);
    // CIAS_LOG_HL("tuya_ir_code_head.ir_head_id = %s", tuya_ir_code_head.ir_head_id);
    // CIAS_LOG_HL("2-ir_temp_offset = 0x%x", ir_temp_offset);
    // CIAS_LOG_HL("tuya_ir_code_head.ir_data_num = %d", tuya_ir_code_head.ir_data_num);
    if (tuya_ir_code_head.ir_available_flag != 0x5A)
    {
        CIAS_LOG_ERR("------the ir is not available...\r\n");
        return OPRT_RECV_ERR;;
    }
    if (tuya_ir_dev_type == IR_AIR_CONDITIONER_ID)
    {
        //读空调红外存储具体数据
        tuya_conditioner_ir_code_data_t tuya_ir_code_data;
        for (int i = 0; i < tuya_ir_code_head.ir_data_num; i++)
        {
            // CIAS_LOG_HL("tuya_ir_data_read_from_flash: tuya_ir_code_head.ir_data_num = %d, ir_temp_offset = 0x%x, cur_num = %d", tuya_ir_code_head.ir_data_num, ir_temp_offset, i);
            hal_flash_read_by_cache(ir_temp_offset, sizeof(tuya_conditioner_ir_code_data_t) - 8, (uint8_t *)&tuya_ir_code_data);
            //CIAS_LOG_HL("tuya_ir_code_data.ir_key_len = %d", tuya_ir_code_data.ir_key_len);
            //CIAS_LOG_HL("tuya_ir_code_data.ir_data_len = %d", tuya_ir_code_data.ir_data_len);
            ir_temp_offset += sizeof(tuya_conditioner_ir_code_data_t) - 8;
            if (tuya_ir_code_data.ir_key_len > 0)
            {
                tuya_ir_code_data.ir_key = cias_malloc(tuya_ir_code_data.ir_key_len);
                if (tuya_ir_code_data.ir_key) //读红外数据key
                {
                    memset(tuya_ir_code_data.ir_key, 0, tuya_ir_code_data.ir_key_len);
                    hal_flash_read_by_cache(ir_temp_offset, tuya_ir_code_data.ir_key_len, tuya_ir_code_data.ir_key);
                    //log_printf("ir_key = %s, tuya_ir_code_data.ir_key = %s\r\n", ir_key, tuya_ir_code_data.ir_key);
                    for (int i = 0; i < strlen(ir_key); i++)
                    {
                        if (ir_key[i] != tuya_ir_code_data.ir_key[i])
                        {
                            find_key_flag = -1;
                            break;
                        }
                        else
                        {
                            find_key_flag = 1;
                        }
                    }
                }
                else
                {
                    CIAS_LOG_HL("tuya_ir_data_find_from_flash:tuya_ir_code_data.ir_key malloc error");
                    return OPRT_RECV_ERR;
                }
                ir_temp_offset += tuya_ir_code_data.ir_key_len;
                if (tuya_ir_code_data.ir_key)
                    free(tuya_ir_code_data.ir_key);
            }
            if (tuya_ir_code_data.ir_data_len > 0)
            {
                tuya_ir_code_data.ir_data = cias_malloc(tuya_ir_code_data.ir_data_len + 1); //解码的时候遇到\0才结束，所以多申请一个元素

                if (tuya_ir_code_data.ir_data) //读红外数据code
                {
                    memset(tuya_ir_code_data.ir_data, 0, tuya_ir_code_data.ir_data_len + 1);
                    // CIAS_LOG_HL("tuya_ir_code_head.ir_head_id = %s", tuya_ir_code_head.ir_head_id);
                    hal_flash_read_by_cache(ir_temp_offset, tuya_ir_code_data.ir_data_len, tuya_ir_code_data.ir_data);

                    if (find_key_flag == 1)
                    {
                        CIAS_LOG_HL("find key......");
                        CIAS_LOG_HL("1--tuya_ir_code_data.ir_data_len = %d", tuya_ir_code_data.ir_data_len);
                        CIAS_LOG_HL("1--tuya_ir_code_data.ir_data = %s", tuya_ir_code_data.ir_data);
                        CIAS_LOG_HL("--tuya_ir_code_head.ir_head_id = %s", tuya_ir_code_head.ir_head_id);
                        CIAS_LOG_HL("--tuya_ir_code_data.ir_data_len = %d", tuya_ir_code_data.ir_data_len);
                        CIAS_LOG_HL("--tuya_ir_code_head.ir_head_id_len = %d", tuya_ir_code_head.ir_head_id_len);
                        cias_local_send_ir(tuya_ir_code_data.ir_data, tuya_ir_code_head.ir_head_id);
                        ret = 0;
                        break;
                    }
                }
                else
                {
                    CIAS_LOG_HL("tuya_ir_data_find_from_flash:tuya_ir_code_data.ir_data malloc error");
                    return OPRT_RECV_ERR;
                }
                ir_temp_offset += tuya_ir_code_data.ir_data_len;
                if (tuya_ir_code_data.ir_data)
                    cias_free(tuya_ir_code_data.ir_data);
            }
        }
    }
    else
    {
        //读非空调红外存储具体数据
        tuya_universal_ir_code_data_t tuya_ir_code_data;
        for (int i = 0; i < tuya_ir_code_head.ir_data_num; i++)
        {
            CIAS_LOG_HL("tuya_ir_data_find_from_flash: tuya_ir_code_head.ir_data_num = %d, ir_temp_offset = 0x%x, cur_num = %d", tuya_ir_code_head.ir_data_num, ir_temp_offset, i);
            hal_flash_read_by_cache(ir_temp_offset, sizeof(tuya_universal_ir_code_data_t) - 4, (uint8_t *)&tuya_ir_code_data);
            CIAS_LOG_HL("tuya_ir_code_data.keyid = %d", tuya_ir_code_data.keyid);
            CIAS_LOG_HL("tuya_ir_code_data.ir_data_len = %d", tuya_ir_code_data.ir_data_len);
            ir_temp_offset += sizeof(tuya_universal_ir_code_data_t) - 4;
            if (tuya_ir_code_data.ir_data_len > 0)
            {
                tuya_ir_code_data.ir_data = cias_malloc(tuya_ir_code_data.ir_data_len + 1);
                if (tuya_ir_code_data.ir_data) //读红外数据code
                {
                    memset(tuya_ir_code_data.ir_data, 0, tuya_ir_code_data.ir_data_len + 1);
                    // CIAS_LOG_HL("tuya_ir_code_head.ir_head_id = %s", tuya_ir_code_head.ir_head_id);
                    hal_flash_read_by_cache(ir_temp_offset, tuya_ir_code_data.ir_data_len, tuya_ir_code_data.ir_data);
                    CIAS_LOG_HL("1--tuya_ir_code_data.ir_data = %s", tuya_ir_code_data.ir_data);
                    CIAS_LOG_HL("--tuya_ir_code_head.ir_head_id = %s", tuya_ir_code_head.ir_head_id);
                    CIAS_LOG_HL("--tuya_ir_code_data.ir_data_len = %d", tuya_ir_code_data.ir_data_len);
                    CIAS_LOG_HL("--tuya_ir_code_head.ir_head_id_len = %d", tuya_ir_code_head.ir_head_id_len);
                    if (ir_key_value == tuya_ir_code_data.keyid)
                    {
                        CIAS_LOG_HL("find key......");
                        cias_local_send_ir(tuya_ir_code_data.ir_data, tuya_ir_code_head.ir_head_id);
                        ret = 0;
                        break;
                    }
                }
                else
                {
                    CIAS_LOG_HL("tuya_ir_data_find_from_flash:tuya_ir_code_data.ir_data malloc error");
                     return OPRT_RECV_ERR;
                }
                ir_temp_offset += tuya_ir_code_data.ir_data_len;
                if (tuya_ir_code_data.ir_data)
                    cias_free(tuya_ir_code_data.ir_data);
            }
        }
    }
    if (tuya_ir_code_head.ir_dev_id)
        cias_free(tuya_ir_code_head.ir_dev_id);
    if (tuya_ir_code_head.ir_head_id)
        cias_free(tuya_ir_code_head.ir_head_id);

    return OPRT_OK;
}
