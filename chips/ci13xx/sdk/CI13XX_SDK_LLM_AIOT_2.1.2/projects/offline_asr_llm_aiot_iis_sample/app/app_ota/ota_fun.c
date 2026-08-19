#include <stdio.h> 
#include <string.h>
#include <malloc.h>
#include "FreeRTOS.h" 
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "ci_flash_data_info.h"
#include "user_config.h"
#include "ci_log.h"
#include "crc.h"
#include "flash_update.h"
#include "ota_config.h"
#include "flash_manage_outside_port.h"
#include "status_share.h"
partition_check_t partition_check;

//extern uint8_t version_val[9];
uint8_t ota_firmware_version[6];
static ota_status_t ota_status; //ota升级分区partiton的信息
void get_ota_version(uint8_t *data)
{
/*     data[0] = version[2];
    data[1] = version[1];
    data[2] = version[0]; */
    memcpy(data, ota_firmware_version, 6);
}

//计算分区表信息的crc
uint16_t get_partition_table_crc(partition_table_t *partition_table)
{
    int len = sizeof(partition_table_t) - 2;
    uint16_t sum = 0;
    uint8_t user_code1_status = partition_table->user_code1_status;
    uint8_t user_code2_status = partition_table->user_code2_status;
    uint8_t asr_cmd_model_status = partition_table->asr_cmd_model_status;
    uint8_t dnn_model_status = partition_table->dnn_model_status;
    uint8_t voice_status = partition_table->voice_status;
    uint8_t user_file_status = partition_table->user_file_status;

    partition_table->user_code1_status = 0xF0;
    partition_table->user_code2_status = (partition_table->FirmwareFormatVer == 1) ? 0xF0 : 0xFF;
    partition_table->asr_cmd_model_status = 0;
    partition_table->dnn_model_status = 0;
    partition_table->voice_status = 0;
    partition_table->user_file_status = 0;
    
    for(int i = 0; i< len; i++)
    {
        sum += ((uint8_t*)partition_table)[i];
    }
    partition_table->user_code1_status = user_code1_status;
    partition_table->user_code2_status = user_code2_status;
    partition_table->asr_cmd_model_status = asr_cmd_model_status;
    partition_table->dnn_model_status = dnn_model_status;
    partition_table->voice_status = voice_status;
    partition_table->user_file_status = user_file_status;
    return sum;
}

void flash_config_to_normal(void)
{
    // scu_run_not_in_flash();
    // flash_init(QSPI0);
    spic_prefetch_en(QSPI0,false);
}


void flash_config_to_xip(void)
{
    scu_run_in_flash();
    // flash_init(QSPI0);
    spic_xipconfig(QSPI0);
}

int32_t post_ota_write_flash(char *buf, uint32_t addr, uint32_t size)
{
	int ret = RETURN_OK;
    if(partition_check == PARTITION_OK)
        post_write_flash(buf, addr, size);
    else
        flash_write(QSPI0, addr,(uint32_t)buf, size);
    return ret;
}

int32_t post_ota_erase_flash(uint32_t addr, uint32_t size)
{
	int ret = RETURN_OK;
    if(partition_check == PARTITION_OK)
        post_erase_flash(addr, size);
    else
        flash_erase(QSPI0, addr,size);
    return ret;
}

int32_t post_ota_read_flash(char *buf, uint32_t addr, uint32_t size)
{
    int ret = RETURN_OK;
    if(partition_check == PARTITION_OK)
        post_read_flash(buf,addr,size);
    else
        flash_read(QSPI0, buf,addr,size);
    return ret;
}

//计算分区的crc信息
static uint16_t get_partition_crc(uint32_t partition_offset, uint32_t partition_size)
{
    uint16_t crc = 0;
    uint32_t offset = 0;
    char *read_buf_for_ota = pvPortMalloc(OTA_CHECK_LENGTH);
    if (!read_buf_for_ota)
        CI_ASSERT(0,"get_partition_crc malloc erro\n");    
    //mprintf("get_partition_crc\r\n");
    while(offset < partition_size)
    {
        uint32_t read_len = partition_size - offset;
        read_len = read_len > OTA_CHECK_LENGTH ? OTA_CHECK_LENGTH:read_len;
        memset(read_buf_for_ota, 0, OTA_CHECK_LENGTH);
        post_ota_read_flash((char *)read_buf_for_ota, partition_offset + offset, read_len);
        crc = crc16_ccitt(crc, read_buf_for_ota, read_len);
        offset += read_len;
    }
    vPortFree(read_buf_for_ota);
    return crc;
}

//计算分区需要擦除的大小
static uint32_t get_erase_partition_size(uint32_t size)
{
    uint32_t earse_page = size/ERASE_4K;
    if (size%ERASE_4K)
        earse_page ++;
    return (earse_page*ERASE_4K);
}
//
static void ota_erase_partiton(uint32_t addr, uint32_t size)
{
    if (size > ERASE_MAX_SIZE)
    {
        int erase_number = size / ERASE_MAX_SIZE;
        for (int i = 0; i < erase_number; i++)
        {
            post_ota_erase_flash(addr + i*ERASE_MAX_SIZE, ERASE_MAX_SIZE);
        }
        if(size % ERASE_MAX_SIZE)
            post_ota_erase_flash(addr + erase_number*ERASE_MAX_SIZE, size - erase_number*ERASE_MAX_SIZE);
    } 
    else
    {
        post_ota_erase_flash(addr, size);
    }
}
//校验所有分区信息是否正确
static bool check_partition_crc(partition_table_t *partition_table)
{
    uint8_t *p = &partition_table->user_code1_version;
    for (int i = UPDATA_USER_CODE1; i < UPDATA_OVER_FLOW; i++)
    {
        if(get_partition_crc(((uint32_t *)p)[1], ((uint32_t *)p)[2]) != ((uint32_t *)p)[3])
        {
            mprintf("partition %d crc error...\r\n", i);
            return false;
        }
        p += sizeof(single_partition_t);
    }
    return true;
}

//打印所有分区表信息
static void print_partition_table(partition_table_t *partition_table)
{
    mprintf("user_code1_offset = 0x%x\n", partition_table->user_code1_offset);
    mprintf("user_code1_size = 0x%x\n", partition_table->user_code1_size);
    mprintf("user_code1_status = 0x%x\n", partition_table->user_code1_status);
    mprintf("user_code2_offset = 0x%x\n", partition_table->user_code2_offset);
    mprintf("user_code2_size = 0x%x\n", partition_table->user_code2_size);
    mprintf("user_code2_status = 0x%x\n", partition_table->user_code2_status);
    mprintf("\n");
/*     mprintf("asr_cmd_model_offset = 0x%x\n", partition_table->asr_cmd_model_offset);
    mprintf("asr_cmd_model_size = 0x%x\n", partition_table->asr_cmd_model_size);
    mprintf("dnn_model_offset = 0x%x\n", partition_table->dnn_model_offset);
    mprintf("dnn_model_size = 0x%x\n", partition_table->dnn_model_size);
    mprintf("voice_offset = 0x%x\n", partition_table->voice_offset);
    mprintf("voice_size = 0x%x\n", partition_table->voice_size);
    mprintf("user_file_offset = 0x%x\n", partition_table->user_file_offset);
    mprintf("user_file_size = 0x%x\n", partition_table->user_file_size); */
}

//更新分区表信息
static void updata_partition_table(partition_table_t *partition_table)
{
    partition_table->patitiontablechecksum = get_partition_table_crc(partition_table);//get_partition_list_checksum(partition_table_in); 
    post_ota_erase_flash(FILECONFIG_SPIFLASH_START_ADDR, ERASE_4K);
    post_ota_write_flash(partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
    //print_partition_table(partition_table);
}

//更新分区表版本信息
static void updata_partition_table_version(partition_table_t *partition_table, partition_table_t *partition_table_new)
{
    partition_table->soft_ware_version = partition_table_new->soft_ware_version;//get_partition_list_checksum(partition_table_in); 
    updata_partition_table(partition_table);
    //print_partition_table(partition_table);
}

//设置一个分区ota成功，更新分区表
void set_ota_partition_new(partition_index_t partition_index, uint32_t version, uint32_t offset, uint32_t size, uint32_t crc)
{
    partition_table_t *partition_table = pvPortMalloc(sizeof(partition_table_t));
    if (!partition_table)
        CI_ASSERT(0,"set_ota_partition_new malloc erro\n");  
    //mprintf("set_ota_partition_new %d\r\n", partition_index);
    post_ota_read_flash( partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
    uint8_t *p = &partition_table->user_code1_version;
    p += (partition_index - 1)*sizeof(single_partition_t);
    ((uint32_t *)p)[0] = version;
    ((uint32_t *)p)[1] = offset;
    ((uint32_t *)p)[2] = size;
    ((uint32_t *)p)[3] = crc;
    p[16] = USER_CODE_AREA_STA_OK;
    updata_partition_table(partition_table);
    vPortFree(partition_table);
}

//设置一个分区ota正在升级，更新分区表
static void set_ota_partition_old(partition_index_t partition_index)
{
    partition_table_t *partition_table = pvPortMalloc(sizeof(partition_table_t));
    if (!partition_table)
        CI_ASSERT(0,"set_ota_partition_old malloc erro\n"); 
    //mprintf("set_ota_partition_old %d\r\n", partition_index); 
    post_ota_read_flash( partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
    uint8_t *index = &partition_table->user_code1_version;
    index += (partition_index - 1)*sizeof(single_partition_t);
    index[sizeof(single_partition_t) - 1] = USER_CODE_AREA_STA_OLD;
    updata_partition_table(partition_table);
    vPortFree(partition_table);
}

//设置下一个需要ota的分区信息
static void start_ota_next_partition(partition_index_t partition_index, uint8_t *data)
{
    ota_status.partition_index = partition_index;
    memcpy(&ota_status.version, data, sizeof(single_partition_t) - 1);  //从分区信息version开始复制4个32bit信息
    ota_status.recv_size = 0;
    mprintf("start_ota_next_partition %d: [%x %x]\r\n", partition_index, ota_status.offset, ota_status.partition_size);
    set_ota_partition_old(partition_index);
    if (partition_index != UPDATA_USER_FILE)
    {
        uint8_t *index = data + 4 + sizeof(single_partition_t);
        uint32_t offset_next = ((uint32_t *)index)[0];
        ota_status.partition_size = offset_next - ota_status.offset;
        ota_erase_partiton(ota_status.offset, ota_status.partition_size);
    }
    else
    {
        ota_status.partition_size = ota_status.size;
        post_ota_erase_flash(ota_status.offset, get_erase_partition_size(ota_status.size));
    }
}

//获取下一个需要ota的分区编号
static partition_index_t set_next_ota_partition(partition_table_t *partition_table, partition_table_t *partition_table_new)
{
    uint8_t *p = &partition_table->user_code1_version;
    uint8_t *p_new = &partition_table_new->user_code1_version;
    if (partition_table->user_code2_offset < partition_table_new->user_code2_offset) //升级user code 变大,首先升级user code2
    {
        mprintf("ota_partition_user_code2 first\r\n");
        start_ota_next_partition(UPDATA_USER_CODE2, &partition_table_new->user_code2_version); 
#if OTA_SEQUENTIAL_UPGRADE
        ota_status.first_updata_user_code2 = true;
        return UPDATA_USER_CODE1;
#else
        return UPDATA_USER_CODE2;
#endif
    }
#if OTA_SEQUENTIAL_UPGRADE
    if (ota_status.partition_index == UPDATA_USER_CODE2 && ota_status.first_updata_user_code2)
    {
        start_ota_next_partition(UPDATA_USER_CODE1, &partition_table_new->user_code1_version); 
        return UPDATA_USER_CODE2;
    }
    else if (ota_status.partition_index == UPDATA_USER_CODE1 && ota_status.first_updata_user_code2)
    {
        start_ota_next_partition(UPDATA_ASR, &partition_table_new->asr_cmd_model_version); 
        return UPDATA_ASR;
    }
    else
    {
        
        int i = ota_status.partition_index;
        if (i == UPDATA_USER_FILE)
            return UPDATA_OVER_FLOW; 
        #if OTA_NO_DNN
        if(i == UPDATA_ASR)
        {
            i ++;
            p_new += i*sizeof(single_partition_t);   
            i ++;
        }
        else
        {
            p_new += i*sizeof(single_partition_t);   
            i ++;
        }
        #else
        p_new += i*sizeof(single_partition_t);   
        i ++;
        #endif
        start_ota_next_partition(i, p_new); 
        return i;
    }
#else
    for (int i = UPDATA_USER_CODE1; i <= UPDATA_USER_FILE; i++)  //从user code 1到voice依次升级
    {
        //if(memcmp(p, p_new, 16) != 0)
        if(get_partition_crc(((uint32_t *)p)[1], ((uint32_t *)p)[2]) != ((uint32_t *)p)[3]) //首先校验本地固件分区本身是否完整
        {
            start_ota_next_partition(i, p_new); 
            return i;
        }
        if((((uint32_t *)p)[1] != ((uint32_t *)p_new)[1]) || (((uint32_t *)p)[3] != ((uint32_t *)p_new)[3])) //其次对比本地固件和ota固件，分区信息是否有变化
        {
            mprintf("start_ota_next_partition %d %x %x\r\n", i, ((uint32_t *)p)[3], ((uint32_t *)p_new)[3]);
            start_ota_next_partition(i, p_new); 
            return i;
        }
        p += sizeof(single_partition_t);
        p_new += sizeof(single_partition_t);
    }
    return UPDATA_OVER_FLOW;
#endif
    
}

//分区表获取成功后，计算每一个单独的分区，在主机端ota包的偏移
static uint16_t pkg_number[6];
static void init_ota_partition_pkg_number(partition_table_t *partition_table, uint16_t pkg_len)
{
    uint16_t partition_pkg_count = 0;
    uint32_t offset = partition_table->user_code1_offset;

    uint8_t *p = &partition_table->user_code2_version;
    pkg_number[0] = (ERASE_4K * 2) / pkg_len; //user code 1分区前面只有8K的备份分区表信息
    mprintf("init_ota_partition_pkg_number \r\n");

    for (int i = UPDATA_USER_CODE1; i < UPDATA_USER_FILE; i++)
    {
        if (p[16] != 0xff)
        {
            mprintf("[ %x %x", offset, ((uint32_t *)p)[1]);
            offset = ((uint32_t *)p)[1] - offset;  //当前分区大小
            mprintf(" %x ]", offset);
            partition_pkg_count = offset / pkg_len;
            offset = ((uint32_t *)p)[1];
        }
        else 
            partition_pkg_count = 0;
        mprintf(" ,%d", partition_pkg_count);
        pkg_number[i] = partition_pkg_count + pkg_number[i-1];
#if OTA_NO_DNN
        if(i == UPDATA_DNN)
        {
            pkg_number[i] = pkg_number[i-1];
        }
#endif
        p += sizeof(single_partition_t);
        mprintf(" , %d\r\n",pkg_number[i]);
    }
    mprintf("\r\n");
}

static uint16_t get_ota_partition_pkg_number(partition_index_t partition_index)
{
    return pkg_number[partition_index - 1];
}

//判断一个分区是否ota完成，获取需要ota的下一个分区，返回下一个分区的包序号和分区编号
int8_t get_ota_partition_status(uint16_t *pkg_next, uint16_t pkg_count, uint16_t pkg_len)
{
    if (ota_status.recv_size < ota_status.partition_size)          //首先判断是否写到了本分区最后一帧
        return 0;
    uint32_t crc;
    int8_t ret;
    partition_index_t partition_index; 
    static partition_table_t *partition_table = NULL;     //0x6000地址的原始分区表信息
    static partition_table_t *partition_table_new = NULL; //ota新接收的分区表信息
    if(!partition_table)
    {
        partition_table = pvPortMalloc(sizeof(partition_table_t));
        if(!partition_table)
        {
            mprintf("ota_write_flash_data malloc erro\r\n");
            return -1; 
        }
    }
    if (!partition_table_new)
    {
        partition_table_new = pvPortMalloc(sizeof(partition_table_t));
        if (!partition_table_new)
        {
            mprintf("ota_write_flash_data malloc erro\r\n");
            return -1;  
        }
    }
   
    //写到了分区最后一帧，
    mprintf("\nend write flash code %x: [%x, %x]\r\n", ota_status.partition_index, ota_status.offset, ota_status.recv_size);
    if(ota_status.partition_index == UPDATA_FILECONFIG_BAK)
    {
        bool partition_table_new_is_ok = true;
        post_ota_read_flash(partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
        post_ota_read_flash(partition_table_new, FILECONFIG_SPIFLASH_BAK_ADDR, sizeof(partition_table_t));
        print_partition_table(partition_table_new);
        if(partition_table_new->user_code1_offset != USER_CODE1_ADDR)
        {
            ret = -2;
            partition_table_new_is_ok = false;
        }    
        if(get_partition_table_crc(partition_table_new) != partition_table_new->patitiontablechecksum) //校验分区表信息是否正确
        {
            ret = -3;
            partition_table_new_is_ok = false;   
        } 
        if(partition_table->hard_ware_version != partition_table_new->hard_ware_version) //校验硬件版本信息是否正确
        {
            ret = -4;
            partition_table_new_is_ok = false;   
            mprintf("hard_ware_version %x : %x\r\n", partition_table->hard_ware_version, partition_table_new->hard_ware_version);
        }    
        if(partition_table_new_is_ok)
        {
            init_ota_partition_pkg_number(partition_table_new, pkg_len);
        }
        else
        {
            post_ota_read_flash(partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
            post_ota_erase_flash(FILECONFIG_SPIFLASH_BAK_ADDR, ERASE_4K);
            post_ota_write_flash(partition_table, FILECONFIG_SPIFLASH_BAK_ADDR, sizeof(partition_table_t));
            mprintf("new partition_table erro, recvoer old %d\r\n", ret);
            return ret;
        }
    }
    else 
    {
        crc = get_partition_crc(ota_status.offset, ota_status.size);
        if(crc != ota_status.crc)
        {
            mprintf("crc erro %x %x\r\n",ota_status.offset, ota_status.crc, crc);
            return -5;
        }
        set_ota_partition_new(ota_status.partition_index, ota_status.version, ota_status.offset, 
                ota_status.size, ota_status.crc);
    }
    post_ota_read_flash(partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
    partition_index = set_next_ota_partition(partition_table, partition_table_new);
    mprintf("set_next_ota_partition %d\r\n", partition_index);
    if (partition_index == UPDATA_OVER_FLOW)
    {
        *pkg_next = pkg_count;
        memcpy(partition_table, partition_table_new, sizeof(partition_table_t));
        updata_partition_table(partition_table);
        //updata_partition_table_version(partition_table, partition_table_new);
    }
    else
    {
        *pkg_next = get_ota_partition_pkg_number(partition_index);
        
    }
    //mprintf("pkg_next %d\r\n", *pkg_next);
    return 1;
}   

//开始ota，设置先写备份分区表
void ota_start()
{
    if (partition_check == PARTITION_OK)
    {
        mprintf("pause_asr\r\n");
        pause_asr(1, 1);
        iwdg_close(HAL_IWDG_BASE);
        
        scu_set_device_reset(NPU_BASE_ADDR);
        flash_init(QSPI0, DISABLE);
        scu_set_dma_mode(DMAINT_SEL_CHANNELALL);
        partition_check = PARTITION_ERRO;
        vTaskSuspendAll();
    }  
    else
    {
        extern TimerHandle_t ota_request_timer;
        if(ota_request_timer)
            xTimerStop(ota_request_timer, 0);
    }
    
    ota_status.first_updata_user_code2 = false;
    ota_status.partition_index = UPDATA_FILECONFIG_BAK;
    ota_status.partition_size = ERASE_4K * 2;
    ota_status.recv_size = 0;
    ota_status.offset = FILECONFIG_SPIFLASH_BAK_ADDR;
    post_ota_erase_flash( ota_status.offset, ota_status.partition_size); 
    mprintf("success\r\n");
}

//写接收到的ota数据到flash
void ota_write_flash_data(const uint8_t *buffer, const uint32_t size)
{
    post_ota_write_flash(buffer, ota_status.offset + ota_status.recv_size, size);
    ota_status.recv_size += size;
    //mprintf("ota_status.recv_size  %x %x\r\n", ota_status.recv_size, ota_status.offset + ota_status.recv_size);
}

void recover_partition_table()
{
    partition_table_t *partition_table_new = pvPortMalloc(sizeof(partition_table_t));
    post_ota_read_flash( partition_table_new, FILECONFIG_SPIFLASH_BAK_ADDR, sizeof(partition_table_t));
    if (!partition_table_new)
        CI_ASSERT(0,"check_ota_partitoin_table_finish malloc erro\n");
    updata_partition_table(partition_table_new);
    vPortFree(partition_table_new);
    dpmu_software_reset_system_config();
}

//校验ota升级分区表和分区是否成功
partition_check_t check_ota_finish()
{
    partition_table_t *partition_table = pvPortMalloc(sizeof(partition_table_t));
    if (!partition_table)
        CI_ASSERT(0,"check_ota_partitoin_table_finish malloc erro\n");
    //读取分区信息表
    mprintf("check_partitoin_table info ");
    post_ota_read_flash(partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
    print_partition_table(partition_table);
    memcpy(ota_firmware_version, &partition_table->soft_ware_version, 3);
    memcpy(&ota_firmware_version[3], &partition_table->hard_ware_version, 3);
    mprintf("soft_version: %d.%d.%d, hard_version %d.%d.%d\r\n", 
        ota_firmware_version[2], ota_firmware_version[1], ota_firmware_version[0], ota_firmware_version[5], ota_firmware_version[4], ota_firmware_version[3]);
    if(get_partition_table_crc(partition_table) != partition_table->patitiontablechecksum) //校验分区表信息是否正确
    {
        recover_partition_table();
    }
    if(!check_partition_crc(partition_table)) //校验ota升级分区是否成功是否成功
        return PARTITION_ERRO; 
    if(partition_table->user_code2_status == 0xff)
    {
        mprintf("updata user code 2 status.........\r\n");
        partition_table->user_code2_status = USER_CODE_AREA_STA_OK;
        post_ota_write_flash(partition_table, FILECONFIG_SPIFLASH_START_ADDR, sizeof(partition_table_t));
    }
    vPortFree(partition_table); 
    mprintf("success \r\n");
    return PARTITION_OK;
}
