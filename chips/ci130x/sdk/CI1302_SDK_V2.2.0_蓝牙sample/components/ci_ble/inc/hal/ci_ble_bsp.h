#ifndef __BSP_HS6220_H__
#define __BSP_HS6220_H__



//-------------RF command---------------------
#define HS6220_R_REGISTER				0x00 // Define read command to register
#define HS6220_W_REGISTER				0x20 // Define write command to register
#define HS6220_R_RX_PAYLOAD				0x61 // Define RX payload register address
#define HS6220_W_TX_PAYLOAD				0xA0 // Define TX payload register address
#define HS6220_FLUSH_TX					0xE1 // Define flush TX register command
#define HS6220_FLUSH_RX					0xE2 // Define flush RX register command
#define HS6220_REUSE_TX_PL				0xE3 // Define reuse TX payload register command
#define HS6220_ACTIVATE					0x50 // Define ACTIVATE features register command
#define HS6220_R_RX_PL_WID				0x60 // Define read RX payload width register command
#define HS6220_W_ACK_PAYLOAD			0xA8 // Define write ACK payload register command
#define HS6220_W_TX_PAYLOAD_NOACK		0xB0 // Define disable TX ACK for one time register command
#define HS6220_NOP						0xFF // Define No Operation, might be used to read status register
#define HS6220_READ_STATUS				0xFF

#define HS6220_ACTIVATE_DATA			0x53

#define HS6220_CMD_CE_HIGH				0xD5
#define HS6220_CMD_CE_LOW				0xD6

///  HS6220 Bank0 Register Addr
/**************************************************/               
#define HS6220_BANK0_CONFIG             0x00 // 'Config' register address
#define HS6220_BANK0_EN_AA              0x01 // 'Enable Auto Acknowledgment' register address
#define HS6220_BANK0_EN_RXADDR          0x02 // 'Enabled RX addresses' register address
#define HS6220_BANK0_PMU_CTL            0x03 // 'Setup address width' register address
#define HS6220_BANK0_SETUP_RETR         0x04 // 'Setup Auto. Retrans' register address
#define HS6220_BANK0_RF_CH          	0x05 // 'HS6220 channel' register address
#define HS6220_BANK0_RF_SETUP       	0x06 // 'HS6220 setup' register address
#define HS6220_BANK0_STATUS             0x07 // 'Status' register address                  
#define HS6220_BANK0_RX_ADDR_P0         0x0A // 'RX address pipe0' register address
#define HS6220_BANK0_ACCESS_ADDR        0x0A
#define HS6220_BANK0_RX_ADDR_P1         0x0B // 'RX address pipe1' register address
#define HS6220_BANK0_BLE_TIM_CNT		0x0B 
#define HS6220_BANK0_RX_ADDR_P2         0x0C // 'RX address pipe2' register address
#define HS6220_BANK0_BLE_CTL            0x0C // 'BLE Ctrol' register address
#define HS6220_BANK0_TX_ADDR            0x10 // 'TX address' register address
#define HS6220_BANK0_TX_ADVA_ADDR       0x10
#define HS6220_BANK0_RX_PW_P0           0x11 // 'RX payload width, pipe0' register address
#define HS6220_BANK0_TX_HEADER			0x11 // 'BLE TX Header' register address
#define HS6220_BANK0_RX_PW_P1           0x12 // 'RX payload width, pipe1' register address
#define HS6220_BANK0_CRC_INIT           0x12
#define HS6220_BANK0_RX_PW_P2           0x13 // 'RX payload width, pipe2' register address
#define HS6220_BANK0_RX_HEADER          0x14
#define HS6220_BANK0_RX_ADVA_ADDR       0x15
#define HS6220_BANK0_STATUS_EXT         0x16
#define HS6220_BANK0_FIFO_STATUS        0x17 // 'FIFO Status Register' register address
#define HS6220_BANK0_CONFIG_EXT         0x18
#define HS6220_BANK0_DYNPD              0x1C // 'Enable dynamic payload length' register address
#define HS6220_BANK0_FEATURE            0x1D // 'Feature' register address
#define HS6220_BANK0_SETUP_VALUE        0x1E
#define HS6220_BANK0_PRE_GURD           0x1F

///   HS6220 Bank1 register
#define HS6220_BANK1_CHIP_ID            0x00
#define HS6220_BANK1_PLL_CTL0           0x01
#define HS6220_BANK1_PLL_CTL1           0x02
#define HS6220_BANK1_CAL_CTL            0x03
#define HS6220_BANK1_STATUS             0x07
#define HS6220_BANK1_STATE              0x08
#define HS6220_BANK1_CHAN               0x09
#define HS6220_BANK1_FDEV               0x0C
#define HS6220_BANK1_DAC_RANGE          0x0D
#define HS6220_BANK1_CTUNING            0x0F
#define HS6220_BANK1_FTUNING            0x10
#define HS6220_BANK1_RX_CTRL            0x11
#define HS6220_BANK1_FAGC_CTRL_1        0x13
#define HS6220_BANK1_DOC_DACI           0x1A
#define HS6220_BANK1_DOC_DACQ           0x1B
#define HS6220_BANK1_AGC_CTRL           0x1C
#define HS6220_BANK1_AGC_GAIN           0x1D
#define HS6220_BANK1_RF_IVGEN       	0x1E
#define HS6220_BANK1_TEST_PKDET         0x1F

#define HS6220_FIFO_MAX_PACK_SIZE		0x20
#define HS6220_FIFO_STA_RX_FULL			0x02
#define HS6220_FIFO_STA_RX_EMPTY		0x01

#define HS6220_STATUS_RX_DR				0x40
#define HS6220_STATUS_TX_DS				0x20
#define HS6220_STATUS_MAX_RT			0x10
#define HS6220_STATUS_TX_FULL			0x01

#define BIT0                            0x01
#define BIT1                            0x02
#define BIT2                            0x04
#define BIT3                            0x08
#define BIT4                            0x10
#define BIT5                            0x20
#define BIT6                            0x40
#define BIT7                            0x80

//config register bit define
#define PRIM_RX                         BIT0
#define PWR_UP                          BIT1
#define CE_REG                          BIT2
#define EN_CRC                          BIT3
#define MASK_MAX_RT                     BIT4
#define MASK_TX_DS                      BIT5
#define MASK_RX_DR                      BIT6
#define GUARD_EN                        BIT7

//pmu_ctl register bit define
#define HS6220_PWRDN_MASK				0x03   
#define HS6220_PWRDN_WORK				0x00 
#define HS6220_PWRDN_SDAND_BY			0x02 
#define HS6220_PWRDN_SLEEP				0x01 

#define DLDO_ENB_REG                    BIT3
#define DLDO_ENB_MN                     BIT4

//DYNPD register bit define
#define SPI4_EN                         BIT3
#define XN297_EN                        BIT4
#define BYPASS_IO                       BIT5
#define REG_OUTPUT_EN                   BIT6
#define REG_OUTPUT                      BIT7

//FEATURE register bit define
#define EN_DYN_ACK                      BIT0
#define EN_ACK_PAY                      BIT1
#define EN_DPL                          BIT2
#define VCO_AMP_TX_MUX                  BIT3
#define BP_GAU                          BIT4
#define SOFT_RST                        BIT5
#define BLE_EN                          BIT6
#define LONG_PACKET_EN                  BIT7

//STATUS_EXT register bit define
#define TX_AEMPTY                       BIT0
#define RX_AFULL                        BIT1
#define LENERR                          BIT2
#define CRCERR                          BIT3

typedef enum {
	HS6220_Bank0 = 0,
	HS6220_Bank1 = 0x80
}HS6220_Bank_TypeDef;

typedef enum {
	HS6220_PRX_Mode = 0,
	HS6220_PTX_Mode,
	HS6220_Carrier_Mode
}HS6220_ModeTypeDef;

typedef enum {
	Rate_1M = 0,
	Rate_2M = 0x80
}HS6220_Rate_TypeDef;



#endif
