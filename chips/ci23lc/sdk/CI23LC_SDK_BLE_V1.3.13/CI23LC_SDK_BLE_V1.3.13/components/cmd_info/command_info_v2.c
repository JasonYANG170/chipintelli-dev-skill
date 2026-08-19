/**
 * @file command_info.c
 * @brief 
 * @version 0.1
 * @date 2019-04-30
 * 
 * @copyright Copyright (c) 2019 Chipintelli Technology Co., Ltd.
 * 
 */


#include "stdio.h"
#include "command_info.h"
#include "dichotomy_find.h"
#include "ci_flash_data_info.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "string.h"
// #include "asr_api.h"
#include "ci_log.h"
#include "romlib_api.h"

#define MAX_COMBINATION_COUNT           16  

#define cmd_info_malloc(x)          pvPortMalloc(x)
#define cmd_info_free(x)            vPortFree(x)


typedef struct cmd_info_variable_st
{
    uint32_t command_number;
    uint32_t special_word_number;
    //uint32_t voice_number;
    command_info_t* p_cmd_table;
    uint32_t cmd_table_size;
    special_wait_count_t* p_special_word_table;
    uint32_t special_word_table_size;
    command_string_index_t* p_cmd_str_index_table;
    uint32_t cmd_str_index_table_size;
    char* p_string_table;
    uint32_t string_table_size;
    //voice_info_t* p_voice_info_table;
    uint32_t voice_patition_addr;
    SemaphoreHandle_t semaphore;
    uint8_t check_number;           // 用于命令词有效性检查
    uint8_t is_changing_model;
} cmd_info_variable_t;


cmd_info_variable_t cmd_info_variable = {0};

static uint32_t cmd_info_change_cur_model_group_inner(uint8_t model_group_id);

uint32_t is_valid_cmd_handle(cmd_handle_t cmd_handle)
{
    return ((cmd_handle != INVALID_HANDLE) && ((((command_info_t*)cmd_handle)->flags & 0xC0) == cmd_info_variable.check_number));
}

uint32_t cmd_info_init(uint32_t cmd_file_addr_in_flash, uint32_t voice_patition_addr, uint8_t model_group_id)
{
    uint32_t ret = cmd_file_reader_init(cmd_file_addr_in_flash);
    if (0 == ret)
    {
        cmd_info_variable.voice_patition_addr = voice_patition_addr;
        uint32_t max_model_group_id = cmd_file_get_max_model_group_id(); //这个函数必须调用,初始化最大命令词数量等
        #if USE_SEPARATE_WAKEUP_EN        
        ret = cmd_info_change_cur_model_group_inner(max_model_group_id);
        #else
        ret = cmd_info_change_cur_model_group_inner(DEFAULT_MODEL_GROUP_ID);
        #endif
    }
    cmd_info_variable.semaphore = xSemaphoreCreateMutex();
    //srand(*((volatile uint32_t*)0xE000E018));   //初始化随机种子。导致复位，将其屏蔽
    return ret;
}

static void * realloc_buffer(void **old_buffer, uint32_t *old_size, uint32_t new_size)
{
    if (*old_size < new_size)
    {
        if (*old_size > 0)
        {
            cmd_info_free(*old_buffer);
        }
        *old_buffer = cmd_info_malloc(new_size);
        if (*old_buffer)
        {
            *old_size = new_size;
        }
        else
        {
            ci_logerr(LOG_CMD_INFO, "not enough memory\n");
        }
    }
	return NULL;
}

_XIF_ static uint32_t cmd_info_change_cur_model_group_inner(uint8_t model_group_id)
{
    uint32_t ret = 1;
    if (0 == cmd_file_change_cur_model_group(model_group_id))
    {
        cmd_info_variable.command_number = cmd_file_get_command_number();
        realloc_buffer((void**)&(cmd_info_variable.p_cmd_table), &(cmd_info_variable.cmd_table_size)\
                        , sizeof(command_info_t) * cmd_file_get_max_command_number());
        cmd_file_read_command_table(cmd_info_variable.p_cmd_table);

        cmd_info_variable.check_number += 0x40;
        for (int i = 0;i < cmd_info_variable.command_number;i++)
        {
            cmd_info_variable.p_cmd_table[i].flags |= cmd_info_variable.check_number;
        }

        realloc_buffer((void**)&(cmd_info_variable.p_cmd_str_index_table), &(cmd_info_variable.cmd_str_index_table_size)\
                        , sizeof(command_string_index_t) * cmd_file_get_max_command_number());
        cmd_file_read_string_index_table(cmd_info_variable.p_cmd_str_index_table);

        cmd_info_variable.special_word_number = cmd_file_get_special_word_number();
        if (cmd_info_variable.special_word_number)
        {
            realloc_buffer((void**)&(cmd_info_variable.p_special_word_table), &(cmd_info_variable.special_word_table_size)\
                        , sizeof(special_wait_count_t) * cmd_file_get_max_special_word_number());
            cmd_file_read_special_word_table(cmd_info_variable.p_special_word_table);
        }

        // int size = cmd_file_get_string_table_size();
        realloc_buffer((void**)&(cmd_info_variable.p_string_table), &(cmd_info_variable.string_table_size), cmd_file_get_max_cmd_string_table_size());
        cmd_file_read_string_table(cmd_info_variable.p_string_table);
    }
    return ret;
}


_XIF_ uint32_t cmd_info_change_cur_model_group(uint8_t model_group_id)
{
    static unsigned char last_mode_group_id = 0;
    uint32_t ret = 1;
    if (last_mode_group_id != model_group_id)
    {
        xSemaphoreTake(cmd_info_variable.semaphore, portMAX_DELAY);
        last_mode_group_id = model_group_id;

        cmd_info_change_cur_model_group_inner(model_group_id);
        ci_loginfo(LOG_CMD_INFO, "change asr mode %d\n",model_group_id);

        uint32_t lg_model_addr;
        uint32_t lg_model_size;
        uint32_t ac_model_addr;
        uint32_t ac_model_size;
        get_current_model_addr(&ac_model_addr, &ac_model_size, &lg_model_addr, &lg_model_size);

        extern int asrtop_asr_system_create_model(unsigned int lg_model_addr,unsigned int lg_model_size,
                                unsigned int ac_model_addr,unsigned int ac_model_size,void* pdata);
        asrtop_asr_system_create_model(0x50000000+lg_model_addr, lg_model_size, 0x50000000+ac_model_addr, ac_model_size, 0);
        xSemaphoreGive(cmd_info_variable.semaphore);
        ret = 0;
    }
    else
    {
        ret = 0;
    }
    return ret;
}


uint32_t cmd_info_get_cur_model_id(uint32_t* p_dnn_id, uint32_t* p_asr_id, uint32_t* p_voice_group_id)
{
    return cmd_file_get_cur_model_id(p_dnn_id, p_asr_id);
}


static int cmd_string_find_callback(void* pValue, int index, void* CallbackPara)
{
    cmd_info_variable_t * p_cmd_info_variable = (cmd_info_variable_t *)CallbackPara;
    char *p_str = p_cmd_info_variable->p_string_table + p_cmd_info_variable->p_cmd_str_index_table[index].command_string_offset;
    int rst = strcmp(pValue, p_str);
    if (rst > 0)
        return 1;
    else if (rst < 0)
        return -1;
    else
        return 0;
}


static int cmd_id_find_callback(void* pValue, int index, void* CallbackPara)
{
    cmd_info_variable_t* p_cmd_info_variable = (cmd_info_variable_t*)CallbackPara;
    uint16_t t_id = p_cmd_info_variable->p_cmd_table[index].command_id;
    int rst = (int)pValue - (int)t_id;
    if (rst > 0)
        return 1;
    else if (rst < 0)
        return -1;
    else
        return 0;
}


cmd_handle_t cmd_info_find_command_by_id(uint16_t cmd_id)
{
    cmd_handle_t ret = (cmd_handle_t)INVALID_HANDLE;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    int rst = dichotomy_find((void*)(int)cmd_id, 0, cmd_info_variable.command_number - 1, cmd_id_find_callback, &cmd_info_variable);
    if (rst >= 0)
    {
        ret = (cmd_handle_t) & (cmd_info_variable.p_cmd_table[rst]);
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

//_XIF_
cmd_handle_t cmd_info_find_command_by_string(const char* cmd_string)
{
    cmd_handle_t ret = (cmd_handle_t)INVALID_HANDLE;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    int rst = dichotomy_find((void*)cmd_string, 0, cmd_info_variable.command_number - 1, cmd_string_find_callback, &cmd_info_variable);
    if (rst >= 0)
    {
        uint16_t cmd_index = cmd_info_variable.p_cmd_str_index_table[rst].index_of_cmd_table;
        ret = (cmd_handle_t)&cmd_info_variable.p_cmd_table[cmd_index];
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

cmd_handle_t cmd_info_find_command_by_semantic_id(uint32_t semantic_id)
{
    cmd_handle_t ret = (cmd_handle_t)INVALID_HANDLE;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    for (int i = 0;i < cmd_info_variable.command_number;i++)
    {
        if (cmd_info_variable.p_cmd_table[i].semantic_id == semantic_id)
        {
            ret = (cmd_handle_t)&cmd_info_variable.p_cmd_table[i];
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint16_t cmd_info_get_command_id(cmd_handle_t cmd_handle)
{
    uint16_t ret = INVALID_SHORT_ID;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        ret = ((command_info_t*)cmd_handle)->command_id;
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

static uint16_t get_command_index(cmd_handle_t cmd_handle)
{
    command_info_t* p = (command_info_t*)cmd_handle;
    return p - cmd_info_variable.p_cmd_table;
}

char* cmd_info_get_command_string(cmd_handle_t cmd_handle)
{
    char *ret = NULL;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        uint16_t cmd_index = get_command_index(cmd_handle);
        for (int i = 0; i < cmd_info_variable.command_number; i++)
        {
            if (cmd_info_variable.p_cmd_str_index_table[i].index_of_cmd_table == cmd_index)
            {
                ret = cmd_info_variable.p_string_table + cmd_info_variable.p_cmd_str_index_table[i].command_string_offset;
                break;
            }
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint8_t cmd_info_get_cmd_score(cmd_handle_t cmd_handle)
{
    uint8_t ret = 0xFF;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        ret = ((command_info_t*)cmd_handle)->score;
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint32_t cmd_info_get_semantic_id(cmd_handle_t cmd_handle)
{
    uint32_t ret = INVALID_LONG_ID;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        ret = ((command_info_t*)cmd_handle)->semantic_id;
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint8_t cmd_info_get_cmd_flag(cmd_handle_t cmd_handle)
{
    uint8_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        ret = ((command_info_t*)cmd_handle)->flags;
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}


uint32_t cmd_info_is_special_word(cmd_handle_t cmd_handle)
{
    uint32_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        if (((command_info_t*)cmd_handle)->flags & CMD_FLAG_SPECIAL_WORD)
        {
            ret = 1;
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint32_t cmd_info_is_wakeup_word(cmd_handle_t cmd_handle)
{
    uint32_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        if (((command_info_t*)cmd_handle)->flags & CMD_FLAG_WAKEUP_WORD)
        {
            ret = 1;
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint32_t cmd_info_is_combo_word(cmd_handle_t cmd_handle)
{
    uint32_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        if (((command_info_t*)cmd_handle)->flags & CMD_FLAG_COMBO_WORD)
        {
            ret = 1;
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint32_t cmd_info_is_expected_word(cmd_handle_t cmd_handle)
{
    uint32_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        if (((command_info_t*)cmd_handle)->flags & CMD_FLAG_EXPECTED_WORD)
        {
            ret = 1;
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

uint32_t cmd_info_is_unexpected_word(cmd_handle_t cmd_handle)
{
    uint32_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        if (((command_info_t*)cmd_handle)->flags & CMD_FLAG_UNEXPECTED_WORD)
        {
            ret = 1;
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}


static int wait_count_find_callback(void* pValue, int index, void* CallbackPara)
{
    cmd_info_variable_t* p_cmd_info_variable = (cmd_info_variable_t*)CallbackPara;
    uint16_t t_id = p_cmd_info_variable->p_special_word_table[index].index_of_cmd_table;
    int rst = (int)pValue - (int)t_id;
    if (rst > 0)
        return 1;
    else if (rst < 0)
        return -1;
    else
        return 0;
}

int32_t cmd_info_get_special_wait_count(cmd_handle_t cmd_handle)
{
    int32_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        uint32_t cmd_index = get_command_index(cmd_handle);
        int rst = dichotomy_find((void*)cmd_index, 0, (int)(cmd_info_variable.special_word_number - 1), wait_count_find_callback, (void*)&cmd_info_variable);
        if (rst >= 0)
        {
            ret = cmd_info_variable.p_special_word_table[rst].wait_count;
        }
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

static uint8_t get_combination_voice_number(voice_info_t* p_voice_info_table, uint16_t start_index, uint16_t end_index)
{
    int number = 1;
    uint16_t group = p_voice_info_table[start_index].voice_group & 0x7FFF;
    for (uint16_t i = start_index+1; i <= end_index; i++)
    {
        if ((p_voice_info_table[i].voice_group & 0x7FFF) == group && \
            !(p_voice_info_table[i].voice_group & 0x8000))
        {
            number++;
        }
        else
        {
            break;
        }
    }
    return number;
}

int32_t cmd_info_get_voice_index(uint16_t start_index, uint16_t end_index, uint8_t select_index, uint16_t *id_buffer, int buffer_length)
{
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return 0;
    }
    uint32_t voice_number = cmd_file_get_voice_number();
    voice_info_t* p_voice_info_table = (voice_info_t*)cmd_info_malloc(sizeof(voice_info_t) * voice_number);
    if (!p_voice_info_table)
    {
        xSemaphoreGive(cmd_info_variable.semaphore);
        ci_logerr(LOG_CMD_INFO, "not enough memory\n");
        return 0;
    }
    cmd_file_read_voice_table(p_voice_info_table);

    uint16_t group = p_voice_info_table[start_index].voice_group;
    voice_select_type_t select_type = (group & 0x4000) ? VOICE_SELECT_RANDOM : VOICE_SELECT_USER;
    uint8_t option_number = (group >> 7) & 0x007F;

    if (option_number <= 0)
    {
        cmd_info_free(p_voice_info_table);
        xSemaphoreGive(cmd_info_variable.semaphore);
        return 0;
    }

    if (select_index >= 0 && select_index < option_number)
    {
        select_index = select_index;
    }
    else if (select_type == VOICE_SELECT_RANDOM)
    {
        select_index = rand() % option_number;
    }
    else
    {
        select_index = 0;
    }

    uint8_t combination_number;

    // 找到选择的起始项
    for (int i = 0; i < select_index; i++)
    {
        combination_number = get_combination_voice_number(p_voice_info_table, start_index, end_index);
        start_index += combination_number;
    }

    combination_number = get_combination_voice_number(p_voice_info_table, start_index, end_index);
    if (combination_number > 0)
    {
        if (combination_number <= MAX_COMBINATION_COUNT)
        {
            combination_number = combination_number > buffer_length ? buffer_length : combination_number;
            for (int i = 0; i < combination_number; i++)
            {
                id_buffer[i] = p_voice_info_table[start_index + i].voice_id;
            }
            cmd_info_free(p_voice_info_table);
            xSemaphoreGive(cmd_info_variable.semaphore);
            return combination_number;
        }
        else
        {
            mprintf("too many combination voice\n");
        }
    }
    cmd_info_free(p_voice_info_table);
    xSemaphoreGive(cmd_info_variable.semaphore);
    return 0;
}

uint32_t cmd_info_get_voice_index_from_handle(cmd_handle_t cmd_handle, uint16_t* start_index, uint16_t* end_index)
{
    uint32_t ret = 0;
    if (pdTRUE != xSemaphoreTake(cmd_info_variable.semaphore, pdMS_TO_TICKS(100)))
    {
        return ret;
    }
    if (is_valid_cmd_handle(cmd_handle))
    {
        *start_index = ((command_info_t*)cmd_handle)->voice_start_index;
        *end_index = ((command_info_t*)cmd_handle)->voice_end_index;
    }
    xSemaphoreGive(cmd_info_variable.semaphore);
    return ret;
}

#if USE_BLE_MOUDLE
/*****************************************获取cmd_info[60000]文件信息****************************************************************************/

cmd_node * cmd_list_init(void)
{
    cmd_node *head ;

    head = (cmd_node *)MASK_ROM_LIB_FUNC->newlibcfunc.malloc_p(sizeof(cmd_node));
    head->next = NULL;
    head->command_info_ble.cmd_id = -1;
   
    if(head == NULL)
    {
        return NULL;
    }

    return head;
}

int8_t cmd_list_add(cmd_node *head,command_info_ble_t command_info_ble)
{
    if((head==NULL)||(command_info_ble.cmd_str==NULL))
    {
        return -1;
    }

    cmd_node *tmp = head;
    cmd_node *node = (cmd_node *)MASK_ROM_LIB_FUNC->newlibcfunc.malloc_p(sizeof(cmd_node));
    if(node == NULL)
    {
        return -1;
    }

    node->command_info_ble = command_info_ble;//数据写入节点
    node->next = NULL;

    if(head->next == NULL)//插入第一个节点
    {
        head->next = node;
        return 0;
    }

    while (1)
    {
        if(tmp->next == NULL)
        {
            tmp->next = node;
            break;
        }
        if(tmp->next->command_info_ble.cmd_id < node->command_info_ble.cmd_id)
        {
            tmp = tmp->next;
            continue;
        }
        else
        {
            node->next = tmp->next;
            tmp->next = node;
            break;
        }
    }       
    return 0;
}

cmd_node* get_list_node(cmd_node *head,uint32_t offset)
{
    cmd_node *data = head;
    for(int i = 0;i<offset;i++)
    {
        if(data->next != NULL)
        {
            data = data->next;
        }
        else
        {
            mprintf("get last!!!\r\n");
            return NULL;
        }
        
    }

    return data;
}

int8_t show_list_node(cmd_node *head)
{
    cmd_node * tmp = NULL;
    if(head == NULL)
    {
        ci_loginfo(LOG_CMD_INFO, "head == NULL\r\n");
        return -1;
    }
    tmp = head->next;
    while(tmp)
    {   
        command_info_ble_t command_info_ble = tmp->command_info_ble;
        ci_loginfo(LOG_CMD_INFO, "id:%d\t semantic_id:%x\t str:%s\t score:%d\t wait_count:%d\t",command_info_ble.cmd_id,\
        command_info_ble.semantic_id,command_info_ble.cmd_str,command_info_ble.score,\
        command_info_ble.special_wait_count);
        int voice_index = 0;
        for(int i=0; i<command_info_ble.voice_info_ble.option_number; i++)
        {
            ci_loginfo(LOG_CMD_INFO, "播报音ID=%d  ",i);
            for(int j=0; j<command_info_ble.voice_info_ble.combination_number[i]; j++)
            {
                ci_loginfo(LOG_CMD_INFO, "%d\t", command_info_ble.voice_info_ble.voice_id_arr[voice_index++]);
            }
        }
        ci_loginfo(LOG_CMD_INFO, "\n");
        tmp = tmp->next;
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    ci_loginfo(LOG_CMD_INFO, "*****************************************************\n");
}

void get_cur_sheet_info(uint8_t modle_id, command_info_v2_sheet_t* command_info_sheet)
{
    cmd_info_change_cur_model_group_inner(modle_id);         //切换网络
    command_info_sheet->cur_model_number = modle_id;
    cmd_file_get_cur_model_id(&command_info_sheet->dnn_id, &command_info_sheet->asr_id);

    cmd_info_variable_t * p_cmd_info_variable = &cmd_info_variable;     //当前网络词条信息表
    command_info_sheet->command_number = p_cmd_info_variable->command_number;
    cmd_node *cmd_head = cmd_list_init();     //初始化词条信息链表

    //*****当前网络音频信息初始化*******
    uint32_t voice_number = cmd_file_get_voice_number();
    voice_info_t* p_voice_info_table = (voice_info_t*)cmd_info_malloc(sizeof(voice_info_t) * voice_number);
    if (!p_voice_info_table)
    {
        ci_logerr(LOG_CMD_INFO, "not enough memory\n");
        return 1;
    }
    cmd_file_read_voice_table(p_voice_info_table);

    //*****获取所有词条相关信息*******
    for(uint32_t index = 0; index < command_info_sheet->command_number; index++)
    {
        char *p_str = p_cmd_info_variable->p_string_table + p_cmd_info_variable->p_cmd_str_index_table[index].command_string_offset;
        cmd_handle_t cmd_handle = cmd_info_find_command_by_string(p_str);

        command_info_ble_t command_info_ble;
        strcpy(command_info_ble.cmd_str, p_str);
        // command_info_ble.cmd_str = p_str;    //调用cmd_file_read_string_table时，指针有被修改风险.
        command_info_ble.cmd_id = cmd_info_get_command_id(cmd_handle);
        command_info_ble.semantic_id = cmd_info_get_semantic_id(cmd_handle);
        command_info_ble.score = cmd_info_get_cmd_score(cmd_handle);
        command_info_ble.wake_up_flag = cmd_info_is_wakeup_word(cmd_handle) ? 1 : 0;
        command_info_ble.special_wait_count = cmd_info_get_special_wait_count(cmd_handle);

        //*****获取音频信息*******
        uint16_t start_index, end_index;
        cmd_info_get_voice_index_from_handle(cmd_handle,&start_index,&end_index);

        uint16_t group = p_voice_info_table[start_index].voice_group;
        voice_select_type_t select_type = (group & 0x4000) ? VOICE_SELECT_RANDOM : VOICE_SELECT_USER;
        uint8_t option_number = (group >> 7) & 0x007F;  //几组播报音ID
        command_info_ble.voice_info_ble.select_type = select_type;
        command_info_ble.voice_info_ble.option_number = option_number;

        if (option_number <= 0)     //未填写播报ID
        {
            // return 0;
        }
        else
        {
            int voice_index = 0;
            // 找到播报选择的起始项
            for (int i = 0; i < option_number; i++)
            {
                uint8_t combination_number = get_combination_voice_number(p_voice_info_table, start_index, end_index);
                command_info_ble.voice_info_ble.combination_number[i] = combination_number;
                for (int j = 0; j < combination_number; j++)
                {
                    command_info_ble.voice_info_ble.voice_id_arr[voice_index++] = p_voice_info_table[start_index + j].voice_id;
                }
                start_index += combination_number;
            }
        }
        // ci_loginfo(LOG_CMD_INFO, "\n");
        cmd_list_add(cmd_head,command_info_ble);
    }

    command_info_sheet->head = cmd_head;
    cmd_info_free(cmd_head);
    //show_list_node(command_info_sheet->head);

}

#define READ_ASR_FLASH_SIZE           512 
void get_asr_uuid_by_asr_id(int asr_id, char * asr_uuid)
{
    uint32_t asr_addr = 0, asr_size = 0;
    get_asr_addr_by_id(asr_id, &asr_addr, &asr_size);   //获取该模型地址及大小
    // ci_loginfo(LOG_CMD_INFO, "asr_id:%d\t asr_addr:%x\t asr_size=%d\n",asr_id,asr_addr,asr_size);

    char value[READ_ASR_FLASH_SIZE] = {0};      //读取asr文件后READ_ASR_FLASH_SIZE字节
    post_read_flash(value, asr_addr+asr_size-READ_ASR_FLASH_SIZE, READ_ASR_FLASH_SIZE);
    for (uint16_t k = 0; k < READ_ASR_FLASH_SIZE; k++)  //循环查找asr_uuid
    {
        if(value[k]=='<'&value[k+1]=='U'&value[k+2]=='S'&value[k+3]=='R'&value[k+4]=='-'\
        &value[k+5]=='I'&value[k+6]=='N'&value[k+7]=='F'&value[k+8]=='O'&value[k+9]=='>')
        {
            k += 11;    //加上'<USR-INFO> '长度
            while(value[k] !='\n')
            {
                int len = strlen(asr_uuid);
                asr_uuid[len] = value[k++];
            }
            int count = strlen(asr_uuid);
            for(int i = 0;i < count;i++)
            {
                asr_uuid[i] = asr_uuid[i] - 2*i + 10;   //解密
            }
            // ci_loginfo(LOG_CMD_INFO, "asr_uuid=%s\t len=%d\r\n",asr_uuid,strlen(asr_uuid));
        }
    }

}

command_info_v2_sheet_t *command_info_sheet = NULL;
void get_command_info_sheet()
{
    uint8_t model_number = cmd_file_get_model_number();     //网络数量
    command_info_sheet = (command_info_v2_sheet_t*)cmd_info_malloc(sizeof(command_info_v2_sheet_t) * model_number);
    for (uint8_t modle_id = 0; modle_id < model_number; modle_id++)
    {
        //******获取asr_uuid信息*************
        char asr_uuid[32] = {'\0'};       //asr文件uuid字符串
        get_asr_uuid_by_asr_id(modle_id,asr_uuid);
        (command_info_sheet+modle_id)->uuid = asr_uuid;
        ci_loginfo(LOG_CMD_INFO, "asr_uuid=%s\t len=%d\r\n",(command_info_sheet+modle_id)->uuid, strlen((command_info_sheet+modle_id)->uuid));

        //******获取该网络词条、播报等信息*************
        get_cur_sheet_info(modle_id, command_info_sheet+modle_id);

        //******保存网络总数信息*************
        (command_info_sheet+modle_id)->model_number = model_number;
    }
    
}

/*****************************************获取cmd_info[60000]文件信息****************************************************************************/
#endif
