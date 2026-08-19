#ifndef _COM_TASK_
#define _COM_TASK_    
void Chip_Uart0_Send(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd);
void Chip_Uart1_Send(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd);
void Chip_Uart2_Send(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd);
#endif
