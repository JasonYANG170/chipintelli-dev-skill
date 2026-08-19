#include "system_msg_deal.h"
#include "ci130x_uart.h"
#include "command_info.h"
#include "com_task.h"
#include "ci_log.h"

void	Chip_Uart0_Send(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd)
{
	#if  USER_BLUETOOTH_UART0
	unsigned char tc_sum=0;


	if(buffer==NULL)
	{
		mprintf("err com send buf\r\n");
		return;
	}
	if(lenth<2)
	{
		mprintf("err com send lenth %d \r\n",lenth);
		return;
	}
	tc_sum = 0;
	while(lenth--)    
	{  
		tc_sum += *buffer;
		UartPollingSenddata(UART0,*buffer++);
	}  
	if(bchecksumflag !=0)
	{
		UartPollingSenddata(UART0,tc_sum);//send sum
	}
   
 
	if(dEnd_cmd !=0)
	{
		UartPollingSenddata(UART0,dEnd_cmd);//send sum
	}
    #else
	unsigned char tc_sum=0;

	if(buffer==NULL)
	{
		mprintf("err com send buf\r\n");
		return;
	}
	if(lenth<2)
	{
		mprintf("err com send lenth %d \r\n",lenth);
		return;
	}
	tc_sum = 0;
	while(lenth--)    
	{  
		tc_sum += *buffer;
		UartPollingSenddata(UART0,*buffer++);
	}  
	if(bchecksumflag !=0)
	{
		UartPollingSenddata(UART0,tc_sum);//send sum
	}
	if(dEnd_cmd !=0)
	{
		UartPollingSenddata(UART0,dEnd_cmd);//send sum
	}
	#endif
}

void	Chip_Uart1_Send(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd)
{
	#if  USER_BLUETOOTH_UART1
	unsigned char tc_sum=0;


	if(buffer==NULL)
	{
		mprintf("err com send buf\r\n");
		return;
	}
	if(lenth<2)
	{
		mprintf("err com send lenth %d \r\n",lenth);
		return;
	}
	tc_sum = 0;
	while(lenth--)    
	{  
		tc_sum += *buffer;
		UartPollingSenddata(UART1,*buffer++);
	}  
	if(bchecksumflag !=0)
	{
		UartPollingSenddata(UART1,tc_sum);//send sum
	}
   
 
	if(dEnd_cmd !=0)
	{
		UartPollingSenddata(UART1,dEnd_cmd);//send sum
	}
    #else
	unsigned char tc_sum=0;

	if(buffer==NULL)
	{
		mprintf("err com send buf\r\n");
		return;
	}
	if(lenth<2)
	{
		mprintf("err com send lenth %d \r\n",lenth);
		return;
	}
    
	tc_sum = 0;
	while(lenth--)    
	{  
		tc_sum += *buffer;
		UartPollingSenddata(UART1,*buffer++);
	}  
	if(bchecksumflag !=0)
	{
		UartPollingSenddata(UART1,tc_sum);//send sum
	}
	if(dEnd_cmd !=0)
	{
		UartPollingSenddata(UART1,dEnd_cmd);//send sum
	}
	#endif
}
  

void	Chip_Uart2_Send(unsigned char *buffer,int lenth,int bchecksumflag,unsigned char dEnd_cmd)
{
	#if  USER_BLUETOOTH_UART2
	unsigned char tc_sum=0;


	if(buffer==NULL)
	{
		mprintf("err com send buf\r\n");
		return;
	}
	if(lenth<2)
	{
		mprintf("err com send lenth %d \r\n",lenth);
		return;
	}
	tc_sum = 0;
	while(lenth--)    
	{  
		tc_sum += *buffer;
		UartPollingSenddata(UART2,*buffer++);
	}  
	if(bchecksumflag !=0)
	{
		UartPollingSenddata(UART2,tc_sum);//send sum
	}
   
 
	if(dEnd_cmd !=0)
	{
		UartPollingSenddata(UART2,dEnd_cmd);//send sum
	}
    #else
	unsigned char tc_sum=0;

	if(buffer==NULL)
	{
		mprintf("err com send buf\r\n");
		return;
	}
	if(lenth<2)
	{
		mprintf("err com send lenth %d \r\n",lenth);
		return;
	}
    
	tc_sum = 0;
	while(lenth--)    
	{  
		tc_sum += *buffer;
		UartPollingSenddata(UART2,*buffer++);
	}  
	if(bchecksumflag !=0)
	{
		UartPollingSenddata(UART2,tc_sum);//send sum
	}
	if(dEnd_cmd !=0)
	{
		UartPollingSenddata(UART2,dEnd_cmd);//send sum
	}
	#endif
}


