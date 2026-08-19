#include "ci_dtrflash.h"
#include "ci_system.h"
#include "ci_dma.h"
#include "romlib_api.h"

#if (0)
extern void dma_channel0_lock(void);
extern void dma_channel0_unlock(void *xHigherPriorityTaskWoken);
#define DMAC_CHANNEL0_LOCK()          dma_channel0_lock();
#define DMAC_CHANNEL0_UNLOCK()        dma_channel0_unlock(NULL);
#else
#define DMAC_CHANNEL0_LOCK()          do{}while(0);
#define DMAC_CHANNEL0_UNLOCK()        do{}while(0);
#endif

#define SPIC_TIMEOUT                  (5000)
#define SPIC_FIFO_ADDR                (0x50000000)    /*cpu在FLASH正常模式下访问该地址、dma在XIP模式下访问该地址、dma在FLASH正常模式下，访问该地址*/
#define SPIC_DMA_FIFO_ADDR            (0x60000000)    //dma在FLASH正常模式下，访问该地址

#define SPIC_CS_HIGH_CYCLE            (1)
#define SPIC_CS_LOW_CYCLE             (1)

#define SPIC_WFIFO_AMFULL_LEVEL       (60)
#define SPIC_RFIFO_AMEMPTY_LEVEL      (30)
#define SPIC_RFIFO_AMFULL_LEVEL       (2)

#define STATE_REG_GM_DONE_INT         (0x1 << 3)

#define SPIC_CMD_SIZE                 (1)
#define SPIC_CMD_EN                   (1)
#define SPIC_PREFETCH_EN              (1)      

static uint8_t spi_dma_buf[512];      /* DMA访问 */

static uint32_t global_flash_sel = 0;     
static uint32_t global_addr_size = 0;     
DMACChannelx dmachanel = DMACChannel0;

typedef enum/*!< 功能:flash fifo时钟与sram时钟频率关系 */
{
    RAM_CLK_MD_0 = 0,/*!< 功能:同频 */
    RAM_CLK_MD_2 = 1,/*!< 功能:2分频 */
    RAM_CLK_MD_4 = 2,/*!< 功能:4分频 */
}ram_clk_md_t;

typedef volatile struct spic_register
{
    uint32_t globe_cfg;/*!< 属性:RW 偏移:0x00 位宽:32 功能:全局配置寄存器0 */
    uint32_t globe1_cfg;/*!< 属性:RW 偏移:0x04 位宽:32 功能:全局配置寄存器1 */
    uint32_t rx_clk_cfg;/*!< 属性:RW 偏移:0x08 位宽:32 功能:接收时钟配置寄存器 */
    uint32_t addr_mask_cfg;/*!< 属性:RW 偏移:0x0C 位宽:32 功能:地址MASK寄存器 */
    /*!< 功能:addr_mask[x]:1 = nomask，0 = mask;例:
    器件大小16MB，器件有效地址24bit,addr_mask = 16'h00FF;
    器件大小32MB，器件有效地址25bit,addr_mask = 16'h01FF; */
    uint32_t gm_cfg;/*!< 属性:RW 偏移:0x10 位宽:32 功能:通用模式配置寄存器0 */
    uint32_t gm1_cfg;/*!< 属性:RW 偏移:0x14 位宽:32 功能:通用模式配置寄存器1 */
    uint32_t gm_addr_cfg;/*!< 属性:RW 偏移:0x18 位宽:32 功能:通用模式读写地址 */
    uint32_t gm_data_szie_cfg;/*!< 属性:RW 偏移:0x1C 位宽:32 功能:通用模式数据大小配置寄存器 */
    uint32_t rd_cfg;/*!< 属性:RW 偏移:0x20 位宽:32 功能:读操作模式配置寄存器0 */
    uint32_t rd1_cfg;/*!< 属性:RW 偏移:0x24 位宽:32 功能:读操作模式配置寄存器1 */
    uint32_t wr_cfg;/*!< 属性:RW 偏移:0x28 位宽:32 功能:写操作模式配置寄存器0(PSRAM用) */
    uint32_t wr1_cfg;/*!< 属性:RW 偏移:0x2C 位宽:32 功能:写操作模式配置寄存器1(PSRAM用) */
    uint32_t fifo_level_cfg;/*!< 属性:RW 偏移:0x30 位宽:32 功能:FIFO水线配置寄存器 */
    uint32_t rw_data0_cfg;/*!< 属性:RW 偏移:0x34 位宽:32 功能:读写数据寄存器0 */
    uint32_t rw_data1_cfg;/*!< 属性:RW 偏移:0x38 位宽:32 功能:读写数据寄存器1 */
    uint32_t int_ctrl_cfg;/*!< 属性:RW 偏移:0x3C 位宽:32 功能:中断控制寄存器 */
    uint32_t state_reg;/*!< 属性:RO 偏移:0x40 位宽:32 功能:状态寄存器 */
}spic_register_t,*spic_register_p;

typedef struct
{
    /*======gm&xip=======*/
    spic_base_config_t base_config;
    /*========gm=========*/
    uint32_t gm_data_size;
    uint32_t gm_write_en;
    uint32_t gm_read_en;
    uint32_t gm_dma_en;
    uint32_t gm_data_store_md;  /*读写数据存放方式：0，fifo；1，rw_data_reg*/
}spic_config_t,*spic_config_p;

/**
 * @brief 切换flash的时钟分频
 */
void spic_change_clk(uint32_t spic_base,flash_clk_div_t clk)
{
    spic_register_p spic = (spic_register_p)spic_base;
    spic->globe_cfg &= ~(0x3 << 0);
    spic->globe_cfg |= (clk << 0);
}

/**
 * @brief 等待spic空闲
 *
 * @return int32_t 0:成功;1:失败;
 */
static int32_t spic_wait_idle(uint32_t spic_base)
{
    spic_register_p spic = (spic_register_p)spic_base;
    //uint32_t timeout_arbt = SPIC_TIMEOUT;
    uint32_t timeout_main_ctrl = SPIC_TIMEOUT;
    //while((spic->state_reg & (0x1 << 7))  && timeout_arbt--);
    while((spic->state_reg & (0x1 << 8)) && timeout_main_ctrl--);
    //if((0 == timeout_arbt) || (0 == timeout_main_ctrl))
    if(0 == timeout_main_ctrl)
    {
        return -1;
    }
    return 0;
}

/**
 * @brief spic是否能够进行模式切换
 *
 * @return int32_t 0:成功;1:失败;
 */
static int32_t spic_ready_change_mode(uint32_t spic_base)
{
    spic_register_p spic = (spic_register_p)spic_base;
    if(spic_wait_idle(spic_base))
    {
        return -1;
    }
    /*总使能关闭才能切换模式*/
    spic->gm_cfg &= ~(0x1 << 0);
    spic->rd_cfg &= ~(0x1 << 0);
    //spic->wr_cfg &= ~(0x1 << 0);
    if(spic_wait_idle(spic_base))
    {
        return -2;
    }
    return 0;
}

/**
 * @brief 通用模式配置
 *
 * @param config 配置结构体指针
 * @return int32_t 0:成功;1:失败;
 */
static int32_t spic_general_mode_config(uint32_t spic_base,spic_config_p config)
{
    spic_register_p spic = (spic_register_p)spic_base;
    spic_base_config_p base_config = &(config->base_config);
    volatile uint32_t value = spic->gm_cfg;
    if((base_config->cmd0 == 0x00) || ((0x00 == base_config->cmd0) && (0x00 == base_config->cmd1)))
    {
        return -1;
    }
    if(spic_ready_change_mode(spic_base))
    {
        return -2;
    }
	/*gm_cfg，地址模式和命令模式相同*/
	value &= ~((0xF << 2) | (0xF << 7) | (0xF << 15) | (0x1 << 20));
    value = ((base_config->cmd_md << 2) | (base_config->cmd_md << 7) | 
             (base_config->data_md << 15) | (global_flash_sel << 20));
    spic->gm_cfg = value;
    /*gm1_cfg*/
    value = ((base_config->cmd0 << 0) | (base_config->cmd1 << 8) | 
             ((global_addr_size - 1) << 24) | ((SPIC_CMD_SIZE - 1) << 26));
    spic->gm1_cfg = value;

    spic->gm_addr_cfg = base_config->addr;
    spic->gm_data_szie_cfg = config->gm_data_size;
    /*gm_cfg*/
    value = spic->gm_cfg;
	value &= ~((0x1 << 1) | (0x1 << 1) | (0x1 << 12) | (0x1 << 13) | (0x1 << 14) | (0x1 << 21) | 
	           (0x1 << 1) | (0x1 << 23) | (0x1F << 24));
    value |= ((SPIC_CMD_EN << 1) | (base_config->addr_en << 6) | (base_config->dummy_en << 12) |
              (config->gm_read_en << 13) | (config->gm_write_en << 14) | (config->gm_dma_en << 21) | 
              (config->gm_data_store_md << 23) | ((base_config->dummy - 1) << 24));
    spic->gm_cfg = value;
    /*总使能最后才能打开*/
    spic->gm_cfg |= (0x1 << 0);
    return 0;
}

/**
 * @brief XIP配置
 *
 * @param config 配置结构体指针
 * @return int32_t 0:成功;1:失败;
 */
int32_t spic_xip_config(uint32_t spic_base,spic_base_config_p base_config)
{
    spic_register_p spic = (spic_register_p)spic_base;
    volatile uint32_t value = spic->rd_cfg;
    if(spic_ready_change_mode(spic_base))
    {
        return -1;
    }
    /*rd_cfg，地址模式和命令模式相同*/
    value &= ~((0x1 << 1) | (0xF < 2) | (0xF << 4) | (0x1 << 11) | (0xF << 12) | (0x1 << 16) | 
	           (0x1F << 17) | (0x1 << 22));
    value = ((SPIC_CMD_EN << 1) | (base_config->cmd_md << 2) | (base_config->cmd_md << 6) | 
             (base_config->dummy_en << 11) | (base_config->data_md << 12) | (global_flash_sel << 16) | 
             ((base_config->dummy - 1) << 17) | (SPIC_PREFETCH_EN << 22));
    spic->rd_cfg = value;
    /*rd1_cfg*/
    value = ((base_config->cmd0 << 0) | (base_config->cmd1 << 8) | 
             ((global_addr_size - 1) << 24) | ((SPIC_CMD_SIZE - 1) << 26));
    spic->rd1_cfg = value;
    /*总使能最后才能打开*/
    spic->rd_cfg |= (0x1 << 0);
    return 0;
}

/**
 * @brief spic向flash发送命令
 *
 * @param spic_base_config 配置结构体指针
 * @return int32_t 0:成功;1:失败;
 */
int32_t spic_cmd(uint32_t spic_base,spic_base_config_p spic_base_config)
{
    spic_config_t config = {0};
    memcpy(&config,spic_base_config,sizeof(spic_base_config_t));
    return spic_general_mode_config(spic_base,&config);
}

/**
 * @brief spic以CPU的方式向flash读数据
 *
 * @param spic_base_config 配置结构体指针
 * @param read_data 数据指针
 * @param read_len 数据长度
 * @return int32_t 0:成功;1:失败;
 */
int32_t spic_read_by_cpu(uint32_t spic_base,spic_base_config_p spic_base_config,uint8_t* read_data,uint32_t read_len)
{
	uint32_t data_reg[2] = {0};
    spic_register_p spic = (spic_register_p)spic_base;
    spic_config_t config = {0};
    memcpy(&config,spic_base_config,sizeof(spic_base_config_t));
    config.gm_data_size = (read_len/4)*4;
    if(read_len <= 8)
	{
    	config.gm_data_store_md = 1;
    	config.gm_data_size = ((read_len/4)+1)*4;
	}
    config.gm_read_en = 1;
    spic->state_reg |= STATE_REG_GM_DONE_INT;
    //spic->gm_cfg |= (0x1 << 22);
    if(spic_general_mode_config(spic_base,&config))
    {
        return -1;
    }
    if(read_len > 8)
    {
    	memcpy(read_data,(void *)SPIC_FIFO_ADDR,(read_len/4)*4);
    }
    while(!(spic->state_reg & STATE_REG_GM_DONE_INT));
    spic->state_reg |= STATE_REG_GM_DONE_INT;
    if(read_len <= 8)
    {
    	data_reg[0] =  spic->rw_data0_cfg;
		data_reg[1] = spic->rw_data1_cfg;
		spic->rw_data0_cfg = 0xFFFFFFFF;
		spic->rw_data1_cfg = 0xFFFFFFFF;
		data_reg[0] = ((data_reg[0]&0xFF)<< 24)|((data_reg[0]&0xFF00)<<8)|((data_reg[0]&0xFF0000)>>8)|((data_reg[0]&0xFF000000)>>24);
		data_reg[1] = ((data_reg[1]&0xFF)<< 24)|((data_reg[1]&0xFF00)<<8)|((data_reg[1]&0xFF0000)>>8)|((data_reg[1]&0xFF000000)>>24);
		memcpy(read_data,data_reg,read_len);
    }
    if(spic_wait_idle(spic_base))
    {
        return -2;
    }
    return 0;
}

/**
 * @brief spic以CPU的方式向flash写数据
 *
 * @param spic_base_config 配置结构体指针
 * @param write_data 数据指针
 * @param write_len 数据长度
 * @return int32_t 0:成功;1:失败;
 */
int32_t spic_write_by_cpu(uint32_t spic_base,spic_base_config_p spic_base_config,uint8_t* write_data,uint32_t write_len)
{
	uint32_t data_reg[2] = {0};
    spic_register_p spic = (spic_register_p)spic_base;
    spic_config_t config = {0};
    //spic->gm_cfg |= (0x1 << 22);
    memcpy(&config,spic_base_config,sizeof(spic_base_config_t));
    if(write_len > 8)
    {
        config.gm_data_size = (write_len/4)*4;
    }
    else
	{
        config.gm_data_size = write_len;
    	config.gm_data_store_md = 1;
    	memcpy(data_reg,write_data,write_len);
    	spic->rw_data0_cfg = ((data_reg[0]&0xFF)<< 24) | ((data_reg[0]&0xFF00)<<8) |
                             ((data_reg[0]&0xFF0000)>>8) | ((data_reg[0]&0xFF000000)>>24);
    	spic->rw_data1_cfg = ((data_reg[1]&0xFF)<< 24) | ((data_reg[1]&0xFF00)<<8) |
                             ((data_reg[1]&0xFF0000)>>8) | ((data_reg[1]&0xFF000000)>>24);
	}
    config.gm_write_en = 1;
    if(spic_general_mode_config(spic_base,&config))
    {
        return -1;
    }
    if(write_len > 8)
    {
    	memcpy((void *)SPIC_FIFO_ADDR,write_data,(write_len/4)*4);
    }
    while(!(spic->state_reg & STATE_REG_GM_DONE_INT));
    spic->state_reg |= STATE_REG_GM_DONE_INT;
    if(spic_wait_idle(spic_base))
    {
        return -2;
    }
    return 0;
}

/**
 * @brief spic以dma的方式flash写/读数据
 *
 * @param spic_base_config 配置结构体指针
 * @param data 数据指针
 * @param len 数据长度
 * @param flag 读写标志
 * @return int32_t 0:成功;1:失败;
 */
int32_t spic_readwrite_by_dma(uint32_t spic_base,spic_base_config_p spic_base_config,uint8_t* data,uint32_t len,flash_flag_t flag)
{
    spic_register_p spic = (spic_register_p)spic_base;
    spic_config_t config = {0};
    memcpy(&config,spic_base_config,sizeof(spic_base_config_t));
    dma_config_t dam_config = {0};

    dam_config.srcaddr = (uint32_t)spi_dma_buf;
    dam_config.destaddr = SPIC_DMA_FIFO_ADDR;
    config.gm_data_size = (len/4)*4;
    config.gm_dma_en = 1;
    if(FLASH_FLAG_READ == flag)
    {
        config.gm_read_en = 1;
        dam_config.srcaddr = SPIC_DMA_FIFO_ADDR;
        dam_config.destaddr = (uint32_t)spi_dma_buf;
        dam_config.flowctrl = P2M_DMA;
    }
    else
    {
        config.gm_write_en = 1;
        memcpy(spi_dma_buf,data,len);
        dam_config.flowctrl = M2P_DMA;
    }

    /*配置DMA*/
    #if SPIC_DMA_MODEFI
    DMAC_CHANNEL0_LOCK();
    spic_dma_transfer_config(srcaddr, destaddr, len / 4);
    #else
    dam_config.busrtsize = BURSTSIZE1;
    dam_config.transferwidth = TRANSFERWIDTH_32b;
    dam_config.transfersize = len / 4;
    DMAC_CHANNEL0_LOCK();
    spic_dma_config(dmachanel,&dam_config);
    #endif

    if(FLASH_FLAG_READ == flag)
    {
        spic->state_reg |= STATE_REG_GM_DONE_INT;
    }
    spic->gm_cfg &= ~(0x1 << 22);
    if(spic_general_mode_config(spic_base,&config))
    {
        return -1;
    }
    while(!(spic->state_reg & STATE_REG_GM_DONE_INT));
    spic->state_reg |= STATE_REG_GM_DONE_INT;
    if(spic_wait_idle(spic_base))
    {
        return -2;
    }
    if(RETURN_ERR == wait_dma_translate_flag(dmachanel,SPIC_TIMEOUT))
    {
        DMAC_CHANNEL0_UNLOCK();
        return -3;
    }
    DMAC_CHANNEL0_UNLOCK();

    if(FLASH_FLAG_READ == flag)
    {
        memcpy(data,spi_dma_buf,(len/4)*4);
    }
    return 0;
}

/**
 * @brief spic控制器初始化
 *
 * @param init 初始化结构体
 * @return int32_t 0:成功;1:失败;
 */
int32_t spic_init(uint32_t spic_base,spic_init_p init)
{
    spic_register_p spic = (spic_register_p)spic_base;
    //配置globe_cfg寄存器
    spic->globe_cfg = ((init->flash_clk_div << 0) | (RAM_CLK_MD_0 << 2));
    //flash_clk_bypass
    #if FLASH_F_SAME_AS_IP_CORE
    spic->globe_cfg |= (0x1 << 13);
    #endif
    //配置globe_cfg1寄存器
    spic->globe1_cfg = ((0x1 << 1) | (SPIC_CS_HIGH_CYCLE << 11) | (SPIC_CS_LOW_CYCLE << 14));
    //配置rx_clk_cfg寄存器
    spic->rx_clk_cfg = ((DELAY_LINE << 4) | (RX_NAGE_SAMPLE << 21));
    //非DTR模式下，接收时钟使用下降沿采样
    #if FLASH_F_SAME_AS_IP_CORE
    spic->rx_clk_cfg |= (0x1 << 21);
    #endif
    //配置addr_mask_cfg寄存器
    spic->addr_mask_cfg = 0xfc0;     /*8M配0xf80，4M配0xfc0，该配置会影响xip模式的使用*/
    //配置int_ctrl_cfg寄存器
    spic->int_ctrl_cfg = 0;
    //配置fifo_level_cfg寄存器
    spic->fifo_level_cfg = ((SPIC_WFIFO_AMFULL_LEVEL << 0) | (SPIC_RFIFO_AMEMPTY_LEVEL << 7) | (SPIC_RFIFO_AMFULL_LEVEL << 13));
    //保存配置信息
    global_flash_sel = init->flash_sel;
    global_addr_size = init->addr_size;
    return 0;
}

/**
 * @brief flash时钟相位调制
 *
 * @param tx_shift 偏移X个core_clk，注意X必须小于core_clk与flash_clk的频率倍数（dtr模式下小于倍数的一半）
 * @param tx_nege_en 配置为1时在shift的基础上再向后平移半个core_clk周期
 * @param rx_shift 偏移X个core_clk，注意X必须小于core_clk与flash_clk的频率倍数（dtr模式下小于倍数的一半）
 * @param rx_nege_en 配置为1时在shift的基础上再向后平移半个core_clk周期
 * @return int32_t 0:成功;1:失败;
 */
int32_t spic_clk_phase_set(uint32_t spic_base,uint32_t tx_shift,uint32_t tx_nege_en,uint32_t rx_shift,uint32_t rx_nege_en)
{
    spic_register_p spic = (spic_register_p)spic_base;
    uint32_t max_shift = 2 * ((spic->globe_cfg & 0x3) + 1);
    md_sel_t mode = (spic->gm_cfg & 0x3C) >> 2;
    if(mode > MD_SEL_RESERVED_2)
    {
        max_shift /= 2;
    }
    if((tx_shift > max_shift) || (rx_shift > max_shift))
    {
        return -1;
    }
    if(mode > MD_SEL_RESERVED_2)
    {
        spic->globe_cfg &= ~(0xF << 8);
        spic->globe_cfg |= (tx_shift << 8) | (tx_nege_en << 11);
    }
    else
    {
        spic->globe_cfg &= ~(0xF << 4);
        spic->globe_cfg |= (tx_shift << 4) | (tx_nege_en << 7);
    }
    spic->rx_clk_cfg &= ~0xF;
    spic->rx_clk_cfg |= (rx_shift << 0) | (rx_nege_en << 3);
    return 0;
}

#if 0
/**
 * @brief flash硬件复位，需要接FLASH器件的reset脚
 *
 * @param enable 0：不复位；1：复位；
 */
void spic_hardware_reset(uint32_t spic_base,uint8_t enable)
{
    spic_register_p spic = (spic_register_p)spic_base;
    if(enable)
    {
        spic->globe1_cfg |= (0x1 << 1);
    }
    else
    {
        spic->globe1_cfg &= ~(0x1 << 1);
    }
}
#endif

/**
 * @brief flash预取开关
 *
 * @param en enable：开启；disable：关闭；
 */
uint32_t spic_prefetch_en(uint32_t spic_base,bool en)
{
    spic_register_p spic = (spic_register_p)spic_base;
	spic->rd_cfg &= ~(0x1 << 22);
	spic->rd_cfg |= (en << 22);
	return 0;
}

/**
 * @brief flash时钟同频开关
 *
 * @param en enable：开启（同频）；disable：关闭（默认2分频）；
 */
int32_t spic_clk_bypass_en(uint32_t spic_base,bool en)
{
    spic_register_p spic = (spic_register_p)spic_base;
    if(en)
    {
        #if FLASH_F_SAME_AS_IP_CORE
        spic->globe_cfg |= (0x1 << 13);
        #endif
        spic->rx_clk_cfg |= (0x1 << 21);
    }
    else
    {
        #if FLASH_F_SAME_AS_IP_CORE
        spic->globe_cfg &= ~(0x1 << 13);
        #endif
        spic->rx_clk_cfg &= ~(0x1 << 21);
        spic->globe_cfg &= ~(0x3 << 0);
        spic->globe_cfg |= (FLASH_CLK_DIV_2 << 0);
    }
	return 0;
}

/**
 * @brief flash的rx_sample配置
 *
 * @param sample：0，上升沿采样。1，下降沿采样。
 */
void spic_rx_sample(uint32_t spic_base,uint8_t sample)
{
    spic_register_p spic = (spic_register_p)spic_base;
    spic->rx_clk_cfg &= ~(0x1 << 21);
    spic->rx_clk_cfg |= (sample << 21);
    
}

/**
 * @brief flash的delay_line配置
 *
 * @param delay：1 ~ 32
 */
void spic_delay_line(uint32_t spic_base,uint8_t delay)
{
    spic_register_p spic = (spic_register_p)spic_base;
    spic->rx_clk_cfg &= ~(0xFF << 4);
    spic->rx_clk_cfg |= (delay << 4);
}

/**
 * @brief flash的delay_line配置
 *
 * @param nege：0、1
 */
void spic_rx_nege_en(uint32_t spic_base,uint8_t nege)
{
    spic_register_p spic = (spic_register_p)spic_base;
    spic->rx_clk_cfg &= ~(0x1 << 3);
    spic->rx_clk_cfg |= (nege << 3);
}