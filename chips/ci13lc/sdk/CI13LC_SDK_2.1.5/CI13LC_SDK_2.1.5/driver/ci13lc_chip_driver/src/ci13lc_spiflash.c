/**
 * @file ci13lc_spiflash.c
 * @brief  SPIFLASH驱动文件
 *
 * @version 0.1
 * @date 2019-03-27
 *
 * @copyright Copyright (c) 2019  Chipintelli Technology Co., Ltd.
 *
 */

#include <string.h>
#include "ci_spiflash.h"
#include "ci_dma.h"
#include "ci_scu.h"
#include "ci_dpmu.h"
#include "ci_core_eclic.h"
#include "ci_dtrflash.h"
#include "romlib_api.h"
#include "ci_log.h"

#define SPIC_MS                     (get_apb_clk() / 1000)   /* 60000000 / 1000 */
#define OTHER_TIMR                  (20UL)                   /* ms */
#define ERASE_64K                   (64 * 1024)
#define ERASE_4K                    (4 * 1024)
#define FLASH_PAGE_SIZE             (256)

#if FLASH_MSF
#define FLASH_SISZE_16M             (16 * 1024 * 1024)
#endif
      
#define SPIC_DUMMY_BYTE             (8)                      /*单线模式时，1个byte数据的dummy cycle是8*/

/*****************************************配置项*******************************************************/

#define SPIFLASH_CPU                (1)               /*!< 数据FIFO：0，DMA模式，1，CPU模式 */
#define SPIFLASH_CMD_QUAD           (MD_SEL_LINE_1)   /*!< 命令模式：0，单线，1，四线 */
#define SPIFLASH_DATA_QUAD          (MD_SEL_LINE_4)   /*!< 数据模式：0，单线，1，四线 */

#define SPIFLASH_NORMAL_READ        (1)               /*!< 0：不使用普通读，1：使用普通读 */

#if (USE_V7 || USE_V5)
#define SPIFLASH_NORMAL_CLK         (FLASH_CLK_DIV_2) /*当前V7的时钟太低，所以暂时改成2分频 */
#else
#define SPIFLASH_NORMAL_CLK         (FLASH_CLK_DIV_4) /*!240M主频，普通模式读写只能4分频 */
#endif

#define SPIFLASH_XIP_CLK            (FLASH_CLK_DIV_2) /*!240M主频，XIP可以2分频 */

#define SPIC_FLASH_SEL              (0)               /*双Flash选择：0或1*/
#define SPIC_ADDR_SIZE              (3)               /*Address的数据长度：单位Bytes*/

#define SPIC_FLASH_DRV0_DRV1        (1)               /*!< FLASH驱动能力，0：25%，1：50%，2:75%，3:100% */

#define FLASH_READ_STATUS           (0)               /*FLASH读状态寄存器开关*/

/******************************************************************************************************/

static uint8_t spic_cmd_quad_flag = MD_SEL_LINE_1;    /*命令模式标志*/
static uint8_t spic_data_quad_flag = MD_SEL_LINE_1;   /*数据模式标志*/

/**
 * @brief SPI保护类型定义
 */
typedef enum{
    SPIC_SOFTWAREPROTECTION        =0,     /*!< 软件保护 */
    SPIC_HARDWAREPROTECTION        =1,     /*!< 保护由硬件决定 WP为低则保护 */
    SPIC_POWERSUPPLYLOCK_DOWN      =2,     /*!< 必须产生上电序列才能写 */
    SPIC_ONETIMEPROGRAM            =3,     /*!< 一次性编程保护，flash被永久性保护 */
    SPIC_RESV                      =-1,    /*!< 保留 */
}spic_status_protect_t;

static uint8_t flash_status_size = 0;
static void flash_set_status_size()
{
    uint8_t jedec_id[3] = {0};
    spic_read_jedec_id(QSPI0,jedec_id);
    if((jedec_id[0] == 0X5E) ||  //ZBIT
       (jedec_id[0] == 0X20) ||  //XM
       (jedec_id[0] == 0X85) ||  //PUYA
       (jedec_id[0] == 0X68))    //BOYA
    {
        flash_status_size = 2; //需要配置2个寄存器写status1和status2
    }
    else
    {
        flash_status_size = 1; //配置1个寄存器写status1和status2
    }

    #if FLASH_READ_STATUS
    mprintf("flash_status_size %d\n",flash_status_size);
    #endif
}

static uint8_t flash_get_status_size()
{
    return flash_status_size;
}

/**
 * @brief 向flash发送命令
 *
 * @param spic spiflash控制器组
 * @param cmd 命令
 *
 * @retval RETURN_OK、RETURN_ERR
 */
static int32_t spic_send_cmd(spic_base_t spic, spic_cmd_code_t cmd)
{
    spic_base_config_t spic_base_config = {0};
	spic_base_config.cmd_md = spic_cmd_quad_flag;
    spic_base_config.data_md = spic_cmd_quad_flag;
    spic_base_config.cmd0 = cmd;
    if(spic_cmd(spic,&spic_base_config))
    {
        RETURN_ERR;
    }
    return RETURN_OK;
}

/**
 * @brief 读flash某些命令的值
 *
 * @param spic spiflash控制器组
 * @param cmd 命令
 * @param value 值缓存区地址
 * @param value_len 值长度
 *
 * @retval RETURN_OK、RETURN_ERR
 */
static int32_t spic_read_cmd_value(spic_base_t spic, spic_cmd_code_t cmd, uint8_t* value, uint8_t value_len)
{
    spic_base_config_t spic_base_config = {0};
    /*有些配置的命令模式和数据模式只能单线*/
    spic_base_config.cmd_md = MD_SEL_LINE_1;
    spic_base_config.data_md = MD_SEL_LINE_1;
    spic_base_config.cmd0 = cmd;
    
    switch (cmd)
    {
        case SPIC_CMD_CODE_READ_UNIQUE_ID:
        {
            spic_base_config.addr = 0x00;
            spic_base_config.addr_en = 1;
            spic_base_config.dummy = SPIC_DUMMY_BYTE;
            spic_base_config.dummy_en = 1;
        }
        break;
        case SPIC_CMD_CODE_RELEASEPOWERDOWN:
        {

            spic_base_config.dummy = SPIC_DUMMY_BYTE * 3;
            spic_base_config.dummy_en = 1;
        }
        break;
	    case SPIC_CMD_CODE_READJEDECID:
	    case SPIC_CMD_CODE_READSTATUSREG1:
        {
            /*数据模式和命令模式相同*/
	        spic_base_config.cmd_md = spic_cmd_quad_flag;
            spic_base_config.data_md = spic_cmd_quad_flag;
        }
	    break;
        default:
        break;
    }

    if(spic_read_by_cpu(spic,&spic_base_config,value,value_len))
    {
        return RETURN_ERR;
    }
    return RETURN_OK;
}

#if FLASH_MSF
/**
 * @brief 读扩展地址寄存器
 *
 * @param spic spiflash控制器组
 * @param cmd 命令
 * @param reg 寄存器值(flash地址的（24 ~ 31）bit,用来访问16M以上的FLASH)
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t spic_read_extend_addr_register(spic_base_t spic,uint8_t* reg)
{
    if(spic_read_cmd_value(spic, SPIC_CMD_CODE_READ_EXTENDED_ADDR_REG, reg, 1))
    {
        return RETURN_ERR;
    }
    return RETURN_OK;
}

/**
 * @brief 写扩展地址寄存器
 *
 * @param spic spiflash控制器组
 * @param reg 寄存器值(flash地址的（24 ~ 31）bit,用来访问16M以上的FLASH)
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t spic_write_extend_addr_register(spic_base_t spic,uint8_t reg)
{
    if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
    {
        return RETURN_ERR;
    }
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = MD_SEL_LINE_1;
    spic_base_config.data_md = MD_SEL_LINE_1;
    spic_base_config.cmd0 = SPIC_CMD_CODE_WRITE_EXTENDED_ADDR_REG;
    if(spic_write_by_cpu(spic,&spic_base_config,reg,1))
    {
        return RETURN_ERR;
    }
    return RETURN_OK;
}
#endif

static int32_t spic_quad_enable(spic_base_t spic,uint8_t cmd_md, uint8_t data_md)
{
    spic_cmd_code_t cmd = SPIC_CMD_CODE_DISABLE_QUAD;
    spic_cmd_quad_flag = MD_SEL_LINE_4;         //用4线模式发命令去切换成单线模式
    if(cmd_md)
    {
    	spic_cmd_quad_flag = MD_SEL_LINE_1;     //用单线模式发命令去切换成4线模式
    	cmd = SPIC_CMD_CODE_ENABLE_QUAD;
    }
    if(spic_send_cmd(spic, cmd))
    {
        return RETURN_ERR;
    }
    spic_cmd_quad_flag = cmd_md;                //配置后续命令模式
    spic_data_quad_flag = data_md;              //配置后续数据模式
    return RETURN_OK;
}

/**
 * @brief 读取状态寄存器
 *
 * @param spic spiflash控制器组
 * @param reg 状态寄存器
 * @param status 读取到的状态值
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_read_status_register(spic_base_t spic,spic_cmd_code_t reg,uint8_t *status)
{
    if(spic_read_cmd_value(spic, reg, status, 1))
    {
        return RETURN_ERR;
    }
    return RETURN_OK;
}

/**
 * @brief 检查BUSY状态
 *
 * @param spic spiflash控制器组
 * @param timeout 超时时间
 *3
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t spic_check_busy(spic_base_t spic,int32_t timeout)
{
    uint8_t status;
    do
    {
        if(RETURN_ERR == spic_read_status_register(spic,
                                                   SPIC_CMD_CODE_READSTATUSREG1,&status))
        {
            return RETURN_ERR;
        }
    } while ((status & (0x1 << 0)) && timeout--);
    if(0 >= timeout)
    {
        return RETURN_ERR;
    }
    else
    {
        return RETURN_OK;
    }
}

/**
 * @brief 写状态寄存器
 *
 * @param spic spiflash控制器组
 * @param reg1 状态寄存器1的值
 * @param reg2 状态寄存器2的值
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t spic_write_status_register(spic_base_t spic,char reg1,char reg2,char reg3)
{
    uint8_t temp[3] = {0};
    if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
    {
        return RETURN_ERR;
    }
    temp[0] = reg1;
    temp[1] = reg2;
    temp[2] = reg3;
    spic_base_config_t spic_base_config = {0};
    /*数据模式和命令模式相同*/
    spic_base_config.cmd_md = spic_cmd_quad_flag;
    spic_base_config.data_md = spic_cmd_quad_flag;
    
    /*部分flash型号写状态寄存器0x01，就能配置status1和status2*/
    if(1 == flash_get_status_size()) 
    {
        spic_base_config.cmd0 = SPIC_CMD_CODE_WRSTATUSREG1;
        if(spic_write_by_cpu(spic,&spic_base_config,temp,2))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == spic_check_busy(spic,OTHER_TIMR * SPIC_MS))
        {
            return RETURN_ERR;
        }
    }
    /*部分flash型号status1和status2需分开配置*/
    else 
    {
        spic_base_config.cmd0 = SPIC_CMD_CODE_WRSTATUSREG1;
        if(spic_write_by_cpu(spic,&spic_base_config,temp,1))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == spic_check_busy(spic,OTHER_TIMR * SPIC_MS))
        {
            return RETURN_ERR;
        }

        if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
        {
            return RETURN_ERR;
        }
        spic_base_config.cmd0 = SPIC_CMD_CODE_WRSTATUSREG2;
        if(spic_write_by_cpu(spic,&spic_base_config,&temp[1],1))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == spic_check_busy(spic,OTHER_TIMR * SPIC_MS))
        {
            return RETURN_ERR;
        }
    }

    if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
    {
        return RETURN_ERR;
    }
    spic_base_config.cmd0 = SPIC_CMD_CODE_WRSTATUSREG3;
    if(spic_write_by_cpu(spic,&spic_base_config,&temp[2],1))
    {
        return RETURN_ERR;
    }
    return RETURN_OK;
}

/**
 * @brief 读取Unique ID
 *
 * @param spic spiflash控制器组
 * @param unique 华邦：64bit，GD：128bit
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_read_unique_id(spic_base_t spic,uint8_t* unique)
{
    /*当flash已经配置成4线命令、4线数据模式时，读Unique ID必须切成单线模式*/
    spic_quad_enable(QSPI0,MD_SEL_LINE_1,MD_SEL_LINE_1);
    uint8_t temp[20] = {0};
    if(spic_read_cmd_value(spic, SPIC_CMD_CODE_READ_UNIQUE_ID, temp, 20))
    {
        return RETURN_ERR;
    }
    memcpy((void*)unique,(void*)temp,16);
    spic_quad_enable(QSPI0, SPIFLASH_CMD_QUAD, SPIFLASH_DATA_QUAD);
    return RETURN_OK;
}

/**
 * @brief 读取jedec ID
 *
 * @param spic spiflash控制器组
 * @param jedec
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_read_jedec_id(spic_base_t spic,uint8_t* jedec)
{
    uint8_t temp[4] = {0};
    if(spic_read_cmd_value(spic, SPIC_CMD_CODE_READJEDECID, temp, 4))
    {
        return RETURN_ERR;
    }
    // jedec[0] = temp[3];
    // jedec[1] = temp[2];
    // jedec[2] = temp[1];
    memcpy((void*)jedec,(void*)temp,3);
    return RETURN_OK;
}

/**
 * @brief 设置FLASH四线模式
 *
 * @param spic spiflash控制器
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_quad_mode(spic_base_t spic)
{
    uint8_t reg1,reg2,reg3;
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG1,&reg1))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG2,&reg2))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG3,&reg3))
    {
        return RETURN_ERR;
    }

    #if FLASH_READ_STATUS
    mprintf("spic_quad_mode before 0x%x 0x%x 0x%x\n",reg1,reg2,reg3);
    #endif

    if(0 == (reg2 & (0x1 << 1)))
    {
        reg2 |= (0x1 << 1);
        if(RETURN_ERR == spic_write_status_register(spic,reg1,reg2,reg3))
        {
            return RETURN_ERR;
        }
    }
    int ret = spic_check_busy(spic,OTHER_TIMR * SPIC_MS);
    
    #if FLASH_READ_STATUS
    reg1,reg2,reg3 = 0;
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG1,&reg1))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG2,&reg2))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG3,&reg3))
    {
        return RETURN_ERR;
    }
    mprintf("spic_quad_mode after 0x%x 0x%x 0x%x\n",reg1,reg2,reg3);
    #endif

    return ret;
}

/**
 * @brief powerdown release
 *
 * @param spic spiflash控制器
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t spic_powerdown_release(spic_base_t spic)
{
    uint8_t ID = 0;
    volatile int32_t timeout;
    if(spic_send_cmd(spic, SPIC_CMD_CODE_POWERDOWN))
    {
        return RETURN_ERR;
    }
    timeout = 0x5FFF;    /*延时等待配置成功*/
    while(timeout--);
    if(spic_read_cmd_value(spic, SPIC_CMD_CODE_RELEASEPOWERDOWN, &ID, 1))
    {
        return RETURN_ERR;
    }
    timeout = 0x5FFF;    /*延时等待配置成功*/
    while(timeout--);
    return RETURN_OK;
}

/**
 * @brief reset flash
 *
 * @param spic spiflash控制器
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_reset(spic_base_t spic)
{
    volatile int32_t timeout;
    /*4线写一次复位*/
    spic_cmd_quad_flag = MD_SEL_LINE_4;
    if(spic_send_cmd(spic, SPIC_CMD_CODE_ENABLERESET))
    {
        return RETURN_ERR;
    }
    if(spic_send_cmd(spic, SPIC_CMD_CODE_RESET))
    {
        return RETURN_ERR;
    }
    timeout = 0x3D70;
    while(timeout--);
    /*单线写一次复位*/
    spic_cmd_quad_flag = MD_SEL_LINE_1;
    if(spic_send_cmd(spic, SPIC_CMD_CODE_ENABLERESET))
    {
        return RETURN_ERR;
    }
    if(spic_send_cmd(spic, SPIC_CMD_CODE_RESET))
    {
        return RETURN_ERR;
    }
    timeout = 0x3D70;
    while(timeout--);
    return RETURN_OK;
}

/**
 * @brief SPIflash保护设置
 *
 * @param spic spiflash控制器
 * @param cmd 保护开关
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_protect(spic_base_t spic,FunctionalState cmd)
{
    uint8_t reg1 = 0,reg2 = 0,reg3 = 0,srp1 = 0,srp0 = 0,ret = 0;
    volatile spic_status_protect_t status = SPIC_RESV;
    if(RETURN_ERR == spic_read_status_register(spic,
                                               SPIC_CMD_CODE_READSTATUSREG1,&reg1))
    {
        return RETURN_ERR;
    }

    if(RETURN_ERR == spic_read_status_register(spic,
                                               SPIC_CMD_CODE_READSTATUSREG2,&reg2))
    {
        return RETURN_ERR;
    }

    if(RETURN_ERR == spic_read_status_register(spic,
                                               SPIC_CMD_CODE_READSTATUSREG3,&reg3))
    {
        return RETURN_ERR;
    }

    #if FLASH_READ_STATUS
    mprintf("spic_protect before 0x%x 0x%x 0x%x\n",reg1,reg2,reg3);
    #endif

    srp0 = (reg1 &(1 << 7))?1:0;
    srp1 = (reg2 &(1 << 0))?1:0;
    status =(spic_status_protect_t)((srp1 << 1) | (srp0 << 0));

    switch (status)
    {
        case SPIC_SOFTWAREPROTECTION:
            ret=0;/*可以正常写，必须发写使能后*/
            break;
        case SPIC_HARDWAREPROTECTION:
            ret=1;/*硬件保护控制，如果/WP 引脚为低电平，不能写*/
            break;
        case SPIC_POWERSUPPLYLOCK_DOWN:
            ret=2;/*直到下一个 power-down power-up时序后，才能被写*/
            if(RETURN_ERR == spic_powerdown_release(spic))
            {
                return RETURN_ERR;
            }
            break;
        case SPIC_ONETIMEPROGRAM:
            ret=3;/*一次性编程保护，不能再写*/
            return ret;
        default:
            ret=0XFF;
            break;
    }
    if(cmd != ENABLE)
    {
          reg1 &= ~(0x7 << 2);//block protec disable
          reg1 &= ~(0x1 << 5);//Top/Bottom Block protect disbale
          reg1 &= ~(0x1 << 6);//Sector /SEC
          reg2 &= ~(0x1 << 6);//CMP =0
          reg2 &= ~(0x7 << 3);//BP2,BP1,BP0
    }
    else
    {
          reg1 &= ~(0xF << 2);
          reg1 |= (0xB << 2);//保护前256K
          reg2 &= ~(0x1 << 6);//CMP =0
    }
    reg3 &= ~(0x3 << 5);
    reg3 |= (SPIC_FLASH_DRV0_DRV1 << 5);

    if(RETURN_ERR == spic_write_status_register(spic,reg1,reg2,reg3))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_check_busy(spic,OTHER_TIMR * SPIC_MS))
    {
        return RETURN_ERR;
    }
    
    #if FLASH_READ_STATUS
    reg1,reg2,reg3 = 0;
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG1,&reg1))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG2,&reg2))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,SPIC_CMD_CODE_READSTATUSREG3,&reg3))
    {
        return RETURN_ERR;
    }
    mprintf("spic_protect after 0x%x 0x%x 0x%x\n",reg1,reg2,reg3);
    #endif

    return ret;
}

/**
 * @brief 擦除FLASH安全寄存器
 *
 * @param spic spiflash控制器
 * @param reg 擦除寄存器
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_erase_security_reg(spic_base_t spic,spic_security_reg_t reg)
{
    if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
    {
        return RETURN_ERR;
    }
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = MD_SEL_LINE_1;
    spic_base_config.data_md = MD_SEL_LINE_1;
    spic_base_config.cmd0 = SPIC_CMD_CODE_ERASE_SECURITY_REG;
    spic_base_config.addr = (reg * 0x1000);
    spic_base_config.addr_en = 1;
    if(spic_cmd(spic,&spic_base_config))
    {
        return RETURN_ERR;
    }
    return spic_check_busy(spic, OTHER_TIMR* SPIC_MS);
}

/**
 * @brief 写FLASH的安全寄存器
 *
 * @param spic spiflash控制器
 * @param reg 擦除寄存器
 * @param buf mem地址
 * @param addr FLASH寄存器地址：华邦(0 - 256),GD(0 - 1024)
 * @param size 写FLASH的字节数：华邦(0 - 256),GD(0 - 1024)
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_write_security_reg(spic_base_t spic,spic_security_reg_t reg,
                              uint32_t buf,uint32_t addr,uint32_t size)
{
#if FLASH_F_SAME_AS_IP_CORE
    spic_clk_bypass_en(spic,DISABLE);
#else
    spic_change_clk(spic, SPIFLASH_NORMAL_CLK);
#endif
    if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
    {
        return RETURN_ERR;
    }
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = MD_SEL_LINE_1;
    spic_base_config.data_md = MD_SEL_LINE_1;
    spic_base_config.cmd0 = SPIC_CMD_CODE_WRITE_SECURITY_REG;
    spic_base_config.addr = (reg * 0x1000) | addr;
    spic_base_config.addr_en = 1;
    if(spic_write_by_cpu(spic,&spic_base_config,(uint8_t *)buf,size))
    {
        return RETURN_ERR;
    }
    return spic_check_busy(spic, OTHER_TIMR * SPIC_MS);
}

/**
 * @brief 读FLASH的安全寄存器
 *
 * @param spic spiflash控制器
 * @param reg 擦除寄存器
 * @param buf mem地址
 * @param addr FLASH寄存器地址：华邦(0 - 256),GD(0 - 1024)
 * @param size 写FLASH的字节数：华邦(0 - 256),GD(0 - 1024)
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_read_security_reg(spic_base_t spic,spic_security_reg_t reg,
                              uint32_t buf,uint32_t addr,uint32_t size)
{
#if FLASH_F_SAME_AS_IP_CORE
    spic_clk_bypass_en(spic,DISABLE);
#else
    spic_change_clk(spic, SPIFLASH_NORMAL_CLK);
#endif
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = MD_SEL_LINE_1;
    spic_base_config.data_md = MD_SEL_LINE_1;
    spic_base_config.cmd0 = SPIC_CMD_CODE_READ_SECURITY_REG;
    spic_base_config.addr = (reg * 0x1000) | addr;
    spic_base_config.addr_en = 1;
    spic_base_config.dummy = SPIC_DUMMY_BYTE;
    spic_base_config.dummy_en = 1;
    if(spic_read_by_cpu(spic,&spic_base_config,(uint8_t *)buf,size))
    {
        return RETURN_ERR;
    }
    return spic_check_busy(spic,OTHER_TIMR * SPIC_MS);
}

/**
 * @brief FLASH的安全寄存器上锁,慎用:上锁之后将导致该安全寄存器不可再此编程
 *
 * @param spic spiflash控制器
 * @param reg 寄存器
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_security_reg_lock(spic_base_t spic,spic_security_reg_t reg)
{
    uint32_t lock = 0x1 << (reg + 2);
    int8_t reg1,reg2,reg3;
    if(RETURN_ERR == spic_read_status_register(spic,
                                               SPIC_CMD_CODE_READSTATUSREG1,&reg1))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,
                                               SPIC_CMD_CODE_READSTATUSREG2,&reg2))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_read_status_register(spic,
                                               SPIC_CMD_CODE_READSTATUSREG3,&reg3))
    {
        return RETURN_ERR;
    }
    reg2 |= lock;
    if(RETURN_ERR == spic_write_status_register(spic,reg1,reg2,reg3))
    {
        return RETURN_ERR;
    }
    return spic_check_busy(spic,OTHER_TIMR * SPIC_MS);
}

/**
 * @brief 擦除FLASH
 *
 * @param spic spiflash控制器
 * @param code 擦除命令
 * @param addr 擦除地址
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_erase(spic_base_t spic,spic_cmd_code_t code,uint32_t addr)
{
    if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
    {
        return RETURN_ERR;
    }
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = spic_cmd_quad_flag;
    spic_base_config.data_md = spic_cmd_quad_flag;
    spic_base_config.cmd0 = code;
    spic_base_config.addr = addr;
    spic_base_config.addr_en = 1;
    if(spic_cmd(spic,&spic_base_config))
    {
        return RETURN_ERR;
    }
    return spic_check_busy(spic,OTHER_TIMR * SPIC_MS);
}

#if SPIC_DMA_MODEFI
void spic_dma_init(DMAC_FLOWCTRL flowctrl)
{
    dma_config_temp config;
    config.flowctrl = flowctrl;
    config.busrtsize = BURSTSIZE1;
    config.transferwidth = TRANSFERWIDTH_32b;
    extern DMACChannelx dmachanel;
    spic_dma_config_temp(dmachanel,&config);
}
#endif //#if SPIC_DMA_MODEFI

/**
 * @brief 写FLASH的某一页
 *
 * @param spic spiflash控制器
 * @param buf mem地址
 * @param addr FLASH地址
 * @param size 写FLASH的字节数
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_quad_write_page(spic_base_t spic,uint32_t buf,uint32_t addr,
                                    uint32_t size)
{
    if(spic_send_cmd(spic, SPIC_CMD_CODE_WRITE_ENABLE))
    {
        return RETURN_ERR;
    }
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = spic_cmd_quad_flag;
    spic_base_config.data_md = spic_data_quad_flag;
    spic_base_config.cmd0 = SPIC_CMD_CODE_PAGEPROGRAM;   
    if((MD_SEL_LINE_1 == spic_cmd_quad_flag) && (MD_SEL_LINE_4 == spic_data_quad_flag))
    {
        spic_base_config.cmd0 = SPIC_CMD_CODE_QUADINPUTPAGEPROGRAM; /*单线命令、4线数据*/
    }
    spic_base_config.addr = addr;
    spic_base_config.addr_en = 1;
#if SPIFLASH_CPU
    if(spic_write_by_cpu(spic,&spic_base_config,(uint8_t*)buf,size))
    {
        return RETURN_ERR;
    }
#else
	if(spic_readwrite_by_dma(spic,&spic_base_config,(uint8_t *)buf,size,FLASH_FLAG_WRITE))
	{
		return RETURN_ERR;
	}
#endif
    return spic_check_busy(spic,OTHER_TIMR * SPIC_MS);
}

#if SPIFLASH_NORMAL_READ
/**
 * @brief 读FLASH的某一块
 *
 * @param spic spiflash控制器
 * @param buf mem地址
 * @param addr FLASH地址
 * @param size 读FLASH的字节数
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t spic_quad_read_page(spic_base_t spic,uint32_t buf,uint32_t addr,
                                    uint32_t size)
{
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = spic_cmd_quad_flag;
    spic_base_config.data_md = spic_data_quad_flag;
    spic_base_config.cmd0 = SPIC_CMD_CODE_FASTREAD;   
    if((MD_SEL_LINE_1 == spic_cmd_quad_flag) && (MD_SEL_LINE_4 == spic_data_quad_flag))
    {
        spic_base_config.cmd0 = SPIC_CMD_CODE_FASTREADQUADOUTPUT;   /*单线命令、4线数据*/
    }
    spic_base_config.addr = addr;
    spic_base_config.addr_en = 1;
    spic_base_config.dummy = SPIC_DUMMY_BYTE;
    if(MD_SEL_LINE_4 == spic_base_config.cmd_md)
    {
    	spic_base_config.dummy = SPIC_DUMMY_BYTE / 4;
    }
    spic_base_config.dummy_en = 1;
#if SPIFLASH_CPU
    if(spic_read_by_cpu(spic,&spic_base_config,(uint8_t*)buf,size))
    {
        return RETURN_ERR;
    }
#else
    if(spic_readwrite_by_dma(spic,&spic_base_config,(uint8_t *)buf,size,FLASH_FLAG_READ))
	{
		return RETURN_ERR;
	}
#endif
    return spic_check_busy(spic,OTHER_TIMR * SPIC_MS);
}
#endif

int32_t spic_xipconfig(spic_base_t spic)
{
    #if FLASH_F_SAME_AS_IP_CORE
    spic_clk_bypass_en(spic,ENABLE);
    #else
    spic_change_clk(spic,SPIFLASH_XIP_CLK);
    #endif
    spic_base_config_t spic_base_config = {0};
    spic_base_config.cmd_md = spic_cmd_quad_flag;
    spic_base_config.data_md = spic_data_quad_flag;
    spic_base_config.cmd0 = SPIC_CMD_CODE_FASTREAD;   
    if((MD_SEL_LINE_1 == spic_cmd_quad_flag) && (MD_SEL_LINE_4 == spic_data_quad_flag))
    {
        spic_base_config.cmd0 = SPIC_CMD_CODE_FASTREADQUADOUTPUT;   /*单线命令、4线数据*/
    }
    spic_base_config.addr_en = 1;
    spic_base_config.dummy = SPIC_DUMMY_BYTE;
    if(MD_SEL_LINE_4 == spic_base_config.cmd_md)
    {
    	spic_base_config.dummy = SPIC_DUMMY_BYTE / 4;
    }
    spic_base_config.dummy_en = 1;
    return spic_xip_config(spic,&spic_base_config);
}
/**
 * @brief FLASH初始化
 *
 * @param spic spiflash控制器
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t flash_init(spic_base_t spic)
{
    dpmu_set_io_driver_strength(SPI0_CS_PAD, 0);
    dpmu_set_io_driver_strength(SPI0_D1_PAD, 0);
    dpmu_set_io_driver_strength(SPI0_D2_PAD, 0);
    dpmu_set_io_driver_strength(SPI0_D0_PAD, 0);
    dpmu_set_io_driver_strength(SPI0_CLK_PAD, 1);
    dpmu_set_io_driver_strength(SPI0_D3_PAD, 0);

    scu_set_device_reset(HAL_GDMA_BASE);           //DMA硬件复位
    scu_set_device_reset_release(HAL_GDMA_BASE);    //DMA复位完成
    scu_set_device_gate(HAL_GDMA_BASE,ENABLE);      //打开DMA时钟
    //eclic_irq_enable(DMA_IRQn);              //DMA中断使能
    extern DMACChannelx dmachanel;
    clear_dma_translate_flag(dmachanel);      //DMA传输完成标志清除，DMA的通道0为DTR FLASH所用

    scu_set_device_reset((uint32_t)spic);         //DTRFLASH控制器硬件复位
    scu_set_device_reset_release((uint32_t)spic);  //DTRFLASH控制器复位完成

    scu_set_div_parameter(HAL_DTRFLASH_BASE,0);
    scu_set_div_parameter(HAL_DTRFLASH_RAM_BASE,0);

    scu_set_device_gate((uint32_t)spic,ENABLE);    //打开DTRFLASH控制器时钟
    // scu_spiflash_no_boot_set();                 //预留
    #if !SPIFLASH_CPU
    #if SPIC_DMA_MODEFI
    spic_dma_stady_init(dmachanel);
    #endif
    #endif
    spic_init_t init;
    init.flash_clk_div = SPIFLASH_NORMAL_CLK;
    init.flash_sel = SPIC_FLASH_SEL;
    init.addr_size = SPIC_ADDR_SIZE;
    spic_init(spic,&init);
    spic_clk_phase_set(spic,0,0,0,RX_NAGE_EN); 
    /*先用单线写一次掉电、复位*/
    spic_cmd_quad_flag = MD_SEL_LINE_1;
    if(RETURN_ERR == spic_powerdown_release(spic))       
    {
        return RETURN_ERR;
    }
    /*再用4线写一次掉电、复位*/
    spic_cmd_quad_flag = MD_SEL_LINE_4;              
    if(RETURN_ERR == spic_powerdown_release(spic))       
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_reset(spic))
    {
        return RETURN_ERR;
    }         
    spic_init(spic,&init);
    spic_clk_phase_set(spic,0,0,0,RX_NAGE_EN); 
    flash_set_status_size();
    if(RETURN_ERR == spic_protect(spic,ENABLE))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == spic_quad_mode(spic))
	{
		return RETURN_ERR;
	}
	spic_quad_enable(QSPI0, SPIFLASH_CMD_QUAD, SPIFLASH_DATA_QUAD);
    return RETURN_OK;
}

/**
 * @brief FLASH擦除
 *
 * @param spic spiflash控制器
 * @param addr 地址
 * @param size 大小
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t flash_policy_erase(spic_base_t spic,uint32_t addr,uint32_t size)
{
    /* other erase */
    uint32_t current_addr = addr;
    uint32_t current_size = size;
    do
    {
        if((0 == (current_addr % ERASE_64K)) && (size >= ERASE_64K))
        {
            current_size = ERASE_64K;
            if(RETURN_ERR == spic_erase(spic,SPIC_CMD_CODE_BLOCKERASE64K,
                                        current_addr))
            {
                return RETURN_ERR;
            }
        }
        else if((0 == (current_addr % ERASE_4K)) && (size >= ERASE_4K))
        {
            current_size = ERASE_4K;
            if(RETURN_ERR == spic_erase(spic,SPIC_CMD_CODE_SECTORERASE4K,
                                        current_addr))
            {
                return RETURN_ERR;
            }
        }
        else
        {
            return RETURN_ERR;
        }
        size -= current_size;
        current_addr += current_size;
    }
    while(size > 0);
    return RETURN_OK;
}

/**
 * @brief spiflash 写规则
 *
 * @param spic spiflash控制器
 * @param buf mem地址
 * @param addr flash地址
 * @param size 大小
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t flash_write_rule(spic_base_t spic,uint32_t buf,uint32_t addr,
                                uint32_t size)
{
    uint32_t write_64word_count = size / 256;
    uint32_t write_1word_count = (size % 256) / 4;
    #if !SPIFLASH_CPU
    #if SPIC_DMA_MODEFI
    spic_dma_init(M2P_DMA);
    #endif
    #endif
    for(int i = 0;i < write_64word_count;i++)
    {
        if(RETURN_ERR == spic_quad_write_page(spic,buf,addr,256))
        {
            return RETURN_ERR;
        }
        buf += 256;
        addr += 256;
    }
    for(int i = 0;i < write_1word_count;i++)
    {
        if(RETURN_ERR == spic_quad_write_page(spic,buf,addr,4))
        {
            return RETURN_ERR;
        }
        buf += 4;
        addr += 4;
    }

    return RETURN_OK;
}

/**
 * @brief 写FLASH
 *
 * @param spic spiflash控制器
 * @param addr FLASH地址
 * @param buf mem地址
 * @param size 大小
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t flash_quad_write(spic_base_t spic,uint32_t addr,uint32_t buf, uint32_t size)
{
    uint8_t temp[256] = {0};
    uint32_t size_word = 0;
    uint32_t page_size_head = 0,page_size_tail = 0,page_count = 0;
    page_size_head = addr % FLASH_PAGE_SIZE;
    if(0 != page_size_head)
    {
        page_size_head = FLASH_PAGE_SIZE - page_size_head;
    }
    if(page_size_head > size)
    {
        page_size_head = size;
    }
	/*剩下不足一页的大小*/
    page_size_tail = ((size - page_size_head) % FLASH_PAGE_SIZE);
	/*页的个数*/
    page_count = ((size - page_size_head) / FLASH_PAGE_SIZE);
    if(0 != page_size_head)
    {
        memcpy((void*)temp,(void*)buf,page_size_head);
		size_word = page_size_head;
        if(page_size_head % 4)
        {
            size_word = ((page_size_head + 4) / 4) * 4;
        }
        for(int i = 0;i < (size_word - page_size_head);i++)
        {
            temp[page_size_head + i] = 0xFF;
        }
        if(RETURN_ERR == flash_write_rule(spic,(uint32_t)temp,addr,size_word))
        {
            return RETURN_ERR;
        }
        buf += page_size_head;
        addr += page_size_head;
    }
    for(int i = 0;i < page_count;i++)
    {
        memcpy((void*)temp,(void*)buf,FLASH_PAGE_SIZE);
        if(RETURN_ERR == flash_write_rule(spic,(uint32_t)temp,addr,FLASH_PAGE_SIZE))
        {
            return RETURN_ERR;
        }
        buf += FLASH_PAGE_SIZE;
        addr += FLASH_PAGE_SIZE;
    }
    if(0 != page_size_tail)
    {
        memcpy((void*)temp,(void*)buf,page_size_tail);
		size_word = page_size_tail;
        if(page_size_tail % 4)
        {
            size_word = ((page_size_tail + 4) / 4) * 4;
        }
        for(int i = 0;i < (size_word - page_size_tail);i++)
        {
            temp[page_size_tail + i] = 0xFF;
        }
        if(RETURN_ERR == flash_write_rule(spic,(uint32_t)temp,addr,size_word))
        {
            return RETURN_ERR;
        }
    }
    return RETURN_OK;
}

#if SPIFLASH_NORMAL_READ
/**
 * @brief 读FLASH
 *
 * @param spic spiflash控制器
 * @param buf mem地址
 * @param addr FLASH地址
 * @param size 大小
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
static int32_t flash_quad_read(spic_base_t spic,uint32_t buf,uint32_t addr,
                        uint32_t size)
{
    uint8_t temp[256] = {0};
    uint32_t size_word = 0;
    uint32_t block_count = 0;
    uint32_t block_size_tail = 0;

    uint32_t size_start = addr % 4;   

    #if !SPIFLASH_CPU
    #if SPIC_DMA_MODEFI
    spic_dma_init(P2M_DMA);
    #endif
    #endif
    
	/*地址不4字节对齐时，先读出4个字节，保证数据正确性*/
    if(0 != size_start)
    {
        /*读数据的地址：addr & 0xFFFFFC*/
        if(RETURN_ERR == spic_quad_read_page(spic,(uint32_t)temp,addr & 0xFFFFFC,4))
        {
            return RETURN_ERR;
        }
        uint32_t real_size = 4 - size_start;
        if(size < real_size)
        {
            real_size = size;
        }
        memcpy((void*)buf,(void*)temp+size_start,real_size);
        buf += real_size;
        addr += real_size;
        size -= real_size;
    }
    /*block个数*/
    block_count = size / FLASH_PAGE_SIZE;
	/*剩余不足1个block的数据长度*/
    block_size_tail = size % FLASH_PAGE_SIZE;

    for(int i = 0;i < block_count;i++)
    {
        if(RETURN_ERR == spic_quad_read_page(spic,(uint32_t)temp,addr,FLASH_PAGE_SIZE))
        {
            return RETURN_ERR;
        }
        memcpy((void*)buf,(void*)temp,FLASH_PAGE_SIZE);
        buf += FLASH_PAGE_SIZE;
        addr += FLASH_PAGE_SIZE;
    }
    if(0 != block_size_tail)
    {
	    size_word = block_size_tail;
        if(block_size_tail % 4)
        {
            size_word = (block_size_tail + 3) & 0xFFFFFFFC;
        }
        if(RETURN_ERR == spic_quad_read_page(spic,(uint32_t)temp,addr,size_word))
        {
            return RETURN_ERR;
        }
        memcpy((void*)buf,(void*)temp,block_size_tail);
    }
    return RETURN_OK;
}
#endif

/**
 * @brief FLASH擦除,兼容32M以上Flash
 *
 * @param spic spiflash控制器
 * @param addr 地址
 * @param size 大小
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t flash_erase(spic_base_t spic,uint32_t addr,uint32_t size)
{
#if FLASH_MSF
    uint32_t extend_addr,flash_addr;
    extend_addr = addr >> 24;
    flash_addr = 0xFFFFFF & addr;
    if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
    {
        return RETURN_ERR;
    }
    if(RETURN_ERR == flash_policy_erase(spic,flash_addr,size))
    {
        return RETURN_ERR;
    }
    return RETURN_OK;
#else
    return flash_policy_erase(spic,addr,size);
#endif
}

/**
 * @brief 写FLASH,兼容32M以上Flash
 *
 * @param spic spiflash控制器
 * @param addr FLASH地址
 * @param buf mem地址
 * @param size 大小
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t flash_write(spic_base_t spic,uint32_t addr,uint32_t buf,uint32_t size)
{
#if FLASH_F_SAME_AS_IP_CORE
    spic_clk_bypass_en(spic,DISABLE);
#else
    spic_change_clk(spic, SPIFLASH_NORMAL_CLK);
#endif

#if FLASH_MSF
    uint32_t extend_addr,mem_addr,flash_addr,write_size,residue_size,block_count;
    residue_size = size;
    mem_addr = buf;
    extend_addr = addr >> 24;
    flash_addr = 0xFFFFFF & addr;
    write_size = FLASH_SISZE_16M - flash_addr;
    if(write_size < residue_size)
    {
        /* 有跨界情况 */
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_write(spic,flash_addr,mem_addr,write_size))
        {
            return RETURN_ERR;
        }
        flash_addr += write_size;
        mem_addr += write_size;
        residue_size -= write_size;
        extend_addr += 1;
    }
    else
    {
        /* 无跨界情况 */
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_write(spic,flash_addr,mem_addr,residue_size))
        {
            return RETURN_ERR;
        }
        return RETURN_OK;
    }
    /* 计算剩余多少块 */
    block_count = residue_size / FLASH_SISZE_16M;
    for(int i = 0;i < block_count;i++)
    {
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_write(spic,flash_addr,mem_addr,FLASH_SISZE_16M))
        {
            return RETURN_ERR;
        }
        flash_addr += FLASH_SISZE_16M;
        mem_addr += FLASH_SISZE_16M;
        residue_size -= FLASH_SISZE_16M;
        extend_addr += 1;
    }
    /* 是否有剩余不满一块的数据 */
    if(residue_size)
    {
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_write(spic,flash_addr,mem_addr,residue_size))
        {
            return RETURN_ERR;
        }
    }
    return RETURN_OK;
#else
    return flash_quad_write(spic,addr,buf,size);
#endif
}

#if SPIFLASH_NORMAL_READ
/**
 * @brief 读FLASH,兼容32M以上Flash
 *
 * @param spic spiflash控制器
 * @param buf mem地址
 * @param addr FLASH地址
 * @param size 大小
 *
 * @retval RETURN_OK
 * @retval RETURN_ERR
 */
int32_t flash_read(spic_base_t spic,uint32_t buf,uint32_t addr,uint32_t size)
{
#if FLASH_F_SAME_AS_IP_CORE
    spic_clk_bypass_en(spic,ENABLE);
#else
    spic_change_clk(spic, SPIFLASH_NORMAL_CLK);
#endif

#if FLASH_MSF
    uint32_t extend_addr,mem_addr,flash_addr,write_size,residue_size,block_count;
    residue_size = size;
    mem_addr = buf;
    extend_addr = addr >> 24;
    flash_addr = 0xFFFFFF & addr;
    write_size = FLASH_SISZE_16M - flash_addr;
    if(write_size < residue_size)
    {
        /* 有跨界情况 */
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_read(spic,mem_addr,flash_addr,write_size))
        {
            return RETURN_ERR;
        }
        flash_addr += write_size;
        mem_addr += write_size;
        residue_size -= write_size;
        extend_addr += 1;
    }
    else
    {
        /* 无跨界情况 */
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_read(spic,mem_addr,flash_addr,residue_size))
        {
            return RETURN_ERR;
        }
        return RETURN_OK;
    }
    /* 计算剩余多少块 */
    block_count = residue_size / FLASH_SISZE_16M;
    for(int i = 0;i < block_count;i++)
    {
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_read(spic,mem_addr,flash_addr,FLASH_SISZE_16M))
        {
            return RETURN_ERR;
        }
        flash_addr += FLASH_SISZE_16M;
        mem_addr += FLASH_SISZE_16M;
        residue_size -= FLASH_SISZE_16M;
        extend_addr += 1;
    }
    /* 是否有剩余不满一块的数据 */
    if(residue_size)
    {
        if(RETURN_ERR == spic_write_extend_addr_register(spic,extend_addr))
        {
            return RETURN_ERR;
        }
        if(RETURN_ERR == flash_quad_read(spic,mem_addr,flash_addr,residue_size))
        {
            return RETURN_ERR;
        }
    }
    return RETURN_OK;
#else
    return flash_quad_read(spic,buf,addr,size);
#endif
}
#endif

