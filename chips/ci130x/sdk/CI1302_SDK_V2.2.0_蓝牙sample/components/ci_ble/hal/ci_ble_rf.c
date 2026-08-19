#include "ci_ble_rf.h"
#include "ci130x_gpio.h"
#include "ci130x_dpmu.h"
#include "ci130x_core_misc.h"
#include "ci_log.h"
#include "FreeRTOS.h"
#define SPI_OUT 0

int rf_sleep_cnt = 1;
int rf_reset_cnt = 15;

void Delay1ms(unsigned int cnt)
{
	volatile unsigned int i, j;
	for (i = 0; i < cnt; i++)
	{
		for (j = 0; j < 18500; j++)
		{
		}
	}
}

void Delay1us(unsigned int cnt)
{
	volatile unsigned int i, j;
	for (i = 0; i < cnt; i++)
	{
		for (j = 0; j < 20; j++)
		{
		}
	}
}

/* OM6220 模拟SPI(3,4线)接口初始化函数
  已经没有硬件CE的控制脚了，
  IRQ脚根据需要进行配置
*/

/**
 * @brief 初始化射频芯片需要的软件SPI引脚
 */
void HS6220_SPI_Init(void)
{
	// mprintf("HS6220_SPI_Init is called...\r\n");
	scu_set_device_gate((uint32_t)PB, ENABLE); // 开启PB时钟
	scu_set_device_gate((uint32_t)PC, ENABLE);
	dpmu_set_io_reuse(HS6220_CSN_PIN_PAD, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	dpmu_set_io_reuse(HS6220_SCK_PIN_PAD, FIRST_FUNCTION);	// 设置引脚功能复用为GPIO
	dpmu_set_io_reuse(HS6220_MOSI_PIN_PAD, FIRST_FUNCTION); // 设置引脚功能复用为GPIO

	dpmu_set_io_direction(HS6220_CSN_PIN_PAD, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	dpmu_set_io_direction(HS6220_SCK_PIN_PAD, DPMU_IO_DIRECTION_OUTPUT);  // 设置引脚功能为输出模式
	dpmu_set_io_direction(HS6220_MOSI_PIN_PAD, DPMU_IO_DIRECTION_OUTPUT); // 设置引脚功能为输出模式

	dpmu_set_io_pull(HS6220_CSN_PIN_PAD, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	dpmu_set_io_pull(HS6220_SCK_PIN_PAD, DPMU_IO_PULL_DISABLE);	 // 设置关闭上下拉
	dpmu_set_io_pull(HS6220_MOSI_PIN_PAD, DPMU_IO_PULL_DISABLE); // 设置关闭上下拉
	// dpmu_set_io_driver_strength(HS6220_MOSI_PIN_PAD,DPMU_IO_DRIVER_STRENGTH_3);

	gpio_set_output_mode(HS6220_CSN_PORT, HS6220_CSN_PIN); // GPIO的pin脚配置成输出模式
	gpio_set_output_mode(HS6220_SCK_PORT, HS6220_SCK_PIN); // GPIO的pin脚配置成输出模式
	gpio_set_output_mode(HS6220_MOSI_PORT, HS6220_MOSI_PIN);
#if (HS6220_SPI_NWIRE == SPI_4_WIRE)
	dpmu_set_io_reuse(HS6220_MISO_PIN_PAD, FIRST_FUNCTION);
	dpmu_set_io_direction(HS6220_MISO_PIN_PAD, DPMU_IO_DIRECTION_INPUT);
	dpmu_set_io_pull(HS6220_MISO_PIN_PAD, DPMU_IO_PULL_DISABLE);
	gpio_set_input_mode(HS6220_MISO_PORT, HS6220_MISO_PIN);
	HS6220_MISO_HIGH;
#endif
	HS6220_MOSI_HIGH; // HS6220_MOSI_LOW;//HS6220_MOSI_HIGH;
	Delay1us(1);
	HS6220_CSN_HIGH;
	Delay1us(1);
	HS6220_SCK_LOW;
	Delay1us(1);
}
// 3线SPI写操作函数
void SPI_3wire_sendByte(unsigned char TxData)
{
	unsigned char i;
	unsigned char data_output_bit;

	// 设置DIO为输出
	gpio_set_output_mode(HS6220_MOSI_PORT, HS6220_MOSI_PIN);
	for (i = 0; i < 8; i++)
	{
		data_output_bit = TxData & 0x80 ? 1 : 0;
		HS6220_SCK_LOW;
		Delay1us(10);
		if (data_output_bit)
		{
			HS6220_MOSI_HIGH;
		}
		else
		{
			HS6220_MOSI_LOW;
		}
		Delay1us(10);
		HS6220_SCK_HIGH;
		Delay1us(10);
		TxData = TxData << 1;
	}

	HS6220_MOSI_LOW;
	Delay1us(10);
	HS6220_SCK_LOW;
	Delay1us(10);
}

// 3线SPI读操作函数
unsigned char SPI_3wire_readByte(void)
{
	unsigned char bit_ctr;
	unsigned char byte = 0;
	unsigned char temp = 0;
	dpmu_set_io_direction(HS6220_MOSI_PIN_PAD, DPMU_IO_DIRECTION_INPUT);
	gpio_set_input_mode(HS6220_MOSI_PORT, HS6220_MOSI_PIN);
	Delay1us(10);
	for (bit_ctr = 0; bit_ctr < 8; bit_ctr++) // output 8-bit
	{
		byte = byte << 1;
		HS6220_SCK_HIGH;
		Delay1us(10);
		temp = gpio_get_input_level_single(HS6220_MOSI_PORT, HS6220_MOSI_PIN);
		byte |= temp;
		HS6220_SCK_LOW;
		Delay1us(10);
	}

	return (byte); // return read byte
}

#if 1
// 4线spi读写---add by hw

static void Delayspi1us(unsigned int cnt)
{
	volatile unsigned int i, j;
	for (i = 0; i < cnt; i++)
	{
	}
}

#define DELAY_SPI Delayspi1us(2)
unsigned char spi_4wire_wrd(unsigned char TxData)
{
	unsigned char bit_ctr;
	unsigned char byte_read = 0;
	unsigned char temp = 0;

	if (TxData == ((unsigned char)0))
	{
		// mprintf("4 wire spi read\n");
		// 读 miso
		DELAY_SPI;
		for (bit_ctr = 0; bit_ctr < 8; bit_ctr++)
		{
			byte_read = byte_read << 1;
			HS6220_SCK_HIGH;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 1);
#endif
			DELAY_SPI;
			temp = gpio_get_input_level_single(HS6220_MISO_PORT, HS6220_MISO_PIN);
#if SPI_OUT
			if (temp)
			{
				gpio_set_output_level_single(PC, pin_3, 1);
			}
			else
			{
				gpio_set_output_level_single(PC, pin_3, 0);
			}

#endif
			byte_read |= temp;
			HS6220_SCK_LOW;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 0);
#endif
			DELAY_SPI;
		}
	}
	else
	{
		// mprintf("4 wire spi write\n");
		// 写
		for (bit_ctr = 0; bit_ctr < 8; bit_ctr++)
		{
			temp = TxData & 0x80 ? 1 : 0;
			HS6220_SCK_LOW;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 0);
#endif
			DELAY_SPI;
			if (temp)
			{
				HS6220_MOSI_HIGH;
#if SPI_OUT
				gpio_set_output_level_single(PA, pin_4, 1);
#endif
			}
			else
			{
				HS6220_MOSI_LOW;
#if SPI_OUT
				gpio_set_output_level_single(PA, pin_4, 0);
#endif
			}
			// DELAY_SPI;
			HS6220_SCK_HIGH;
#if SPI_OUT
			gpio_set_output_level_single(PA, pin_3, 1);
#endif
			DELAY_SPI;
			TxData = TxData << 1;
		}
		HS6220_MOSI_LOW;
#if SPI_OUT
		gpio_set_output_level_single(PA, pin_4, 0);
#endif
		DELAY_SPI;
		HS6220_SCK_LOW;
#if SPI_OUT
		gpio_set_output_level_single(PA, pin_3, 0);
#endif
		DELAY_SPI;
	}
	return ((unsigned char)byte_read);
}
#endif

// 封装起来的对外可调用的SPI写函数
void SPI_SendByte(unsigned char data)
{
#if (HS6220_SPI_NWIRE == SPI_3_WIRE)
	// 3线SPI写
	SPI_3wire_sendByte(data);
#else
	// 4线SPI读写
	spi_4wire_wrd(data);
#endif
}

// 封装起来的对外可调用的SPI读函数
unsigned char SPI_ReadByte(void)
{
#if (HS6220_SPI_NWIRE == SPI_3_WIRE)
	// 3线SPI读
	return SPI_3wire_readByte();
#else
	// 4线SPI读
	return spi_4wire_wrd(0);
#endif
}

// OM6220写一个寄存器一个值操作函数
void HS6220_write_byte(unsigned char addr, unsigned char D)
{
	HS6220_CSN_LOW;
	Delay1us(1);
	SPI_SendByte(HS6220_W_REGISTER | addr);
	SPI_SendByte(D);
	HS6220_CSN_HIGH;
	Delay1us(1);
}

// OM6220写一个寄存器多个数值操作函数
void HS6220_wr_buffer(unsigned char addr, const unsigned char *buf, unsigned char len)
{
	HS6220_CSN_LOW;
	Delay1us(10);
	SPI_SendByte(HS6220_W_REGISTER | addr);
	while (len--)
	{
		SPI_SendByte(*buf++);
	}
	HS6220_CSN_HIGH;
	Delay1us(10);
}

// OM6220读一个寄存器一个值操作函数
unsigned char HS6220_read_byte(unsigned char addr)
{
	unsigned char rxdata;

	HS6220_CSN_LOW;
	Delay1us(10);
	SPI_SendByte(HS6220_R_REGISTER | addr);
	rxdata = SPI_ReadByte();
	HS6220_CSN_HIGH;
	Delay1us(10);

	return (rxdata);
}

// OM6220读一个寄存器多个值操作函数
void HS6220_read_buffer(unsigned char addr, unsigned char *buf, unsigned char len)
{
	HS6220_CSN_LOW;
	SPI_SendByte(HS6220_R_REGISTER | addr);

	while (len--)
	{
		*buf++ = SPI_ReadByte();
	}
	HS6220_CSN_HIGH;
}

// OM6220直接发命令函数
void HS6220_Operation(unsigned char opt)
{
	HS6220_CSN_LOW;
	Delay1us(10);
	SPI_SendByte(opt);
	HS6220_CSN_HIGH;
	Delay1us(10);
}

/*OM6220 读写寄存器函数
 cmd = code;
 D = data
 */
void HS6220_wr_cmd(unsigned char cmd, unsigned char D)
{
	HS6220_CSN_LOW;
	Delay1us(10);
	SPI_SendByte(cmd);
	SPI_SendByte(D);
	HS6220_CSN_HIGH;
	Delay1us(10);
}

// 切换OM6220 BANK
void HS6220_Bank_Switch(HS6220_Bank_TypeDef bank)
{
	unsigned char sta;

	sta = HS6220_read_byte(HS6220_BANK0_STATUS);
	if (bank != HS6220_Bank0)
	{
		if (!(sta & HS6220_Bank1))
		{
			HS6220_wr_cmd(HS6220_ACTIVATE, HS6220_ACTIVATE_DATA);
		}
	}
	else
	{
		if (sta & HS6220_Bank1)
		{
			HS6220_wr_cmd(HS6220_ACTIVATE, HS6220_ACTIVATE_DATA);
		}
	}
}

// OM6220 切换速率
void HS6220_Change_Rate(HS6220_Rate_TypeDef rate)
{
	unsigned char tmp;

	tmp = HS6220_read_byte(HS6220_BANK0_RF_SETUP);

	if (rate == Rate_1M)
	{
		tmp &= 0xf7;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
	}
	else if (rate == Rate_2M)
	{
		tmp |= 0x08;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
	}
}

/*
 * @brief 切换信道
 *
 * @param chn 值为2-80，表示蓝牙芯片发送频段为2402-2480Mhz，每个频道间隔2Mhz
 *
 */
void ble_change_channel(unsigned char chn)
{
	if (chn < 0x80)
		HS6220_write_byte(HS6220_BANK0_RF_CH, chn);
}

/*
 * @brief 切换功率
 *
 * @param pwr 值为0-7，表示蓝牙芯片发送功率为-43db到8db
 *
 */

void HS6220_Change_Pwr(HS6220_Pwr_TypeDef pwr)
{
	unsigned char tmp;

	tmp = HS6220_read_byte(HS6220_BANK0_RF_SETUP);
	tmp &= ~0x47; // 1000111->10111000
	// tmp |= 0X80;
	switch (pwr)
	{
	case Pwr_8db:
		tmp |= 0x47;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_5db:
		tmp |= 0x40;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_4db:
		tmp |= 0x07;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_0db:
		tmp |= 0x03;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n6db:
		tmp |= 0x01;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n12db:
		tmp |= 0x01;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n16db:
		tmp |= 0x00;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	case Pwr_n43db:
		tmp |= 0x00;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;

	default: // 默认就设置成0db
		tmp |= 0x03;
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		break;
	}
}

/*
 * @brief 切换载波模式
 *
 * @param mod 1位普通模式，2位载波模式
 *
 */
void ble_change_mode(HS6220_ModeTypeDef mod)
{
	unsigned char tmp;

	tmp = HS6220_read_byte(HS6220_BANK0_CONFIG);

	if (mod == HS6220_Carrier_Mode)
	{
		tmp |= 0X80 | HS6220_read_byte(HS6220_BANK0_RF_SETUP); // 0x80 | HS6220_read_byte(HS6220_BANK0_RF_SETUP);
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
	}
}

// OM6220 切换地址
void HS6220_Change_Addr(unsigned char *buf, unsigned char len)
{
	if (len <= 6)
		HS6220_wr_buffer(HS6220_BANK0_RX_ADDR_P0, buf, len); // set address
}

/*OM6220 接收数据包函数
buf：读取到的数据存放的地方
return：正常读取之后，返回的是读取到的数据包长度
*/
unsigned char HS6220_ReceivePack(unsigned char *buf, unsigned char dlen)
{
	unsigned char sta;
	unsigned char fifo_sta;
	unsigned char len;

	sta = HS6220_read_byte(HS6220_BANK0_STATUS);

	if (HS6220_STATUS_RX_DR & sta)
	{
		do
		{
			len = HS6220_read_byte(HS6220_R_RX_PL_WID);

			if ((len <= HS6220_FIFO_MAX_PACK_SIZE)&&(len <= dlen))
			{
				HS6220_read_buffer(HS6220_R_RX_PAYLOAD, buf, len);
			}
			else
			{
				HS6220_Flush_Rx();
			}

			fifo_sta = HS6220_read_byte(HS6220_BANK0_FIFO_STATUS);
		} while (!(fifo_sta & HS6220_FIFO_STA_RX_EMPTY)); /*not empty continue read out*/

		HS6220_write_byte(HS6220_BANK0_STATUS, sta); /*clear irq*/
		HS6220_Clear_All_Irq();
		HS6220_Flush_Tx();
		HS6220_Flush_Rx();
		return len;
	}

	if (sta & (HS6220_STATUS_RX_DR | HS6220_STATUS_TX_DS | HS6220_STATUS_MAX_RT))
	{
		HS6220_write_byte(HS6220_BANK0_STATUS, sta); /*clear irq*/
	}
#if 1
	/* 以下的复位RF和让RF睡眠唤醒的动作是必须的，不可缺少 */
	if (rf_reset_cnt == 0) // 如果300ms都没有收到1包数据，则复位RF重新初始化
	{

		HS6220_Init();
		HS6220_write_byte(HS6220_BANK0_FEATURE, 0x10);
		HS6220_write_byte(HS6220_BANK0_EN_AA, 0x00);
		HS6220_write_byte(HS6220_BANK0_CONFIG, 0xfa);
		// HS6220_write_byte(HS6220_BANK0_RX_PW_P0,10);
		HS6220_write_byte(HS6220_BANK0_RX_PW_P0, rx_lenth);
		HS6220_write_byte(HS6220_BANK0_RF_CH, 5);
		HS6220_write_byte(HS6220_BANK0_EN_RXADDR, 0x03);
		HS6220_CE_High();

		// rf_reset_cnt=rf_reset_time;
	}

	if (rf_sleep_cnt == 0) // 如果20ms都没有收到1包数据，则让RF睡眠唤醒一次
	{

		HS6220_CE_Low();
		HS6220_Flush_Rx();
		HS6220_Clear_All_Irq();

		HS6220_write_byte(HS6220_BANK0_PMU_CTL, 0xae);
		Delay1ms(1); // 这里的1ms延时不可缺少，也不能小
		HS6220_write_byte(HS6220_BANK0_PMU_CTL, 0xac);
		Delay1ms(1); // 这里的1ms延时不可缺少，也不能小
		HS6220_CE_High();
		rf_sleep_cnt = 10;
	}
	/* 以上的复位RF和让RF睡眠唤醒的动作是必须的，不可缺少 */
#endif
	return 0;
}

/* OM6220发送数据包函数
  buf:要发送的数据包内容指针
  len:数据包的长度，从1到32字节
  cmd: 发送数据包的命令，HS6220_W_TX_PAYLOAD和HS6220_W_TX_PAYLOAD_NOACK
*/
void HS6220_SendPack(unsigned char cmd, unsigned char *buf, unsigned char len)
{
	unsigned char sta;
#if 0
	for(int i = 0;i < 10;i++)
	{
		mprintf("buf[%d]:%d\n",i,buf[i]);
	}
#endif
	sta = HS6220_read_byte(HS6220_BANK0_STATUS);
	if (!(sta & HS6220_STATUS_TX_FULL))
	{
		// mprintf("sta:%d",sta);
		HS6220_wr_buffer(cmd, buf, len);
	}
	else
		mprintf("tx_full\n");
}

// OM6220 切换RF工作模式函数
void HS6220_ModeSwitch(HS6220_ModeTypeDef mod)
{
	unsigned char tmp;

	tmp = HS6220_read_byte(HS6220_BANK0_CONFIG);
	if (mod != HS6220_PRX_Mode)
	{
		tmp &= 0xFE;
	}
	else
	{
		tmp |= 0x01;
	}
	HS6220_write_byte(HS6220_BANK0_CONFIG, tmp);

	if (mod == HS6220_Carrier_Mode)
	{
		tmp = 0Xc7; // 0x80 | HS6220_read_byte(HS6220_BANK0_RF_SETUP);
		HS6220_write_byte(HS6220_BANK0_RF_SETUP, tmp);
		/* 注意：最高位为载波使能位，设置了载波模式之后，CE要拉高，载波才会出来；
		   如果不要切换频点，功率等，就不要去在操作RF，否则会看不到载波信号的*/
	}
}

// OM6220 清中断标志
void HS6220_Clear_All_Irq(void)
{
	HS6220_write_byte(HS6220_BANK0_STATUS, 0x70);
}

// OM6220 清空TX FIFO
void HS6220_Flush_Tx(void)
{
	HS6220_Operation(HS6220_FLUSH_TX);
}

// OM6220 清空RX FIFO
void HS6220_Flush_Rx(void)
{
	HS6220_Operation(HS6220_FLUSH_RX);
}

// OM6220 CE拉高
void ble_ce_high(void)
{
	HS6220_Operation(HS6220_CMD_CE_HIGH);
	;
}
void HS6220_CE_High(void)
{
	HS6220_Operation(HS6220_CMD_CE_HIGH);
	;
}
// OM6220 CE拉低
void HS6220_CE_Low(void)
{
	HS6220_Operation(HS6220_CMD_CE_LOW);
}

// 读取MO6220接收到的数据包长度的函数
unsigned char HS6220_read_payload_length(void)
{
	return HS6220_read_byte(HS6220_R_RX_PL_WID);
}

static unsigned char Tx_Payload[32];
static unsigned char Rx_Payload[32];
static unsigned char Tx_Ack_Payload[32];
static unsigned char Rx_Ack_Payload[32];

#define AD_TYPE_MANUFACTURE_DATA 0xff
#define AD_TYPE_SHORT_LOCAL_NAME 0x08
#define AD_TYPE_COMPLETE_LOCAL_NAME 0x09
#define DEFAULT_MANUFACTURE_ID 0xFFF1

static const unsigned char channel_index[3] = {
	2,
	26,
	80};

#define APP_GAP_APPEARANCE 961

