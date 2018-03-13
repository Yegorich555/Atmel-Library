/*
* YlibAvr.c
*
* Created: 22-Sep-17 16:51:32
* Author : yahor.halubchyk
*/

#define F_CPU 9600UL*1000
#include <avr/io.h>
#include <extensions.h>

//************** protocol
//#define UTB_REPEATER_ONLY 1
//************** soft uart
//#define USOFT_TXEN UTB_REPEATER_ONLY == 1 || FLASHEND > 1024
#define USOFT_BAUD 4800
#define USOFT_IO_RX B, 3
#define USOFT_IO_TX B, 2
#define USOFT_BUFFER_EN false

#include <uart_soft.h>
#include <uart_techBrain.h>

#define IO_OUT1 B, 5 
#define IO_OUT2 B, 4 

//unsigned char myByte;
//bool isNew = false;

int main(void)
{
	//set output
	io_set(DDR, IO_OUT1);
	io_set(DDR, IO_OUT2);
	usoft_init();
	utb_init();

	//while(1)   //test
	//{
	//utb_byteReceived(utb_startByte); //start
	//
	//utb_byteReceived(225); //crc
	//utb_byteReceived(1);   //from
	//utb_byteReceived(utb_cmdByte);	//startCmd
	//utb_byteReceived(utb_commonAddr);   //to
	//utb_byteReceived(1);   //repeatCount
	//utb_byteReceived('o'); //output - cmd
	//utb_byteReceived('a'); //for all
	//utb_byteReceived(100); //set out(-s)
	//
	//utb_byteReceived(utb_endByte);
	//utb_cmd_e _cmd = utb_getCmd(utb_buffer);
	//}
	//return;

	//while(1)
	//{
		//usoft_listen();
		//if (isNew)
		//{
			//usoft_putChar(myByte);
			//isNew = false;
		//}
	//}
	//return;
	//usoft_putChar('h');
	while (1)
	{
		usoft_listen();
		utb_cmd_e _cmd = utb_getCmd();
	    
		#pragma GCC diagnostic ignored "-Wswitch"
		switch (_cmd)
		{
			case none:
			break;
			
			case set_out1:
			io_setPort(IO_OUT1);
			break;
			
			case reset_out1:
			io_resetPort(IO_OUT1);
			break;
			
			case set_out2:
			io_setPort(IO_OUT2);
			break;
			
			case reset_out2:
			io_resetPort(IO_OUT2);
			break;
			
			case set_outs:
			io_setPort(IO_OUT1);
			io_resetPort(IO_OUT1);
			break;
			
			case reset_outs:
			io_resetPort(IO_OUT1);
			io_resetPort(IO_OUT2);
			break;
		}
		#pragma GCC diagnostic warning  "-Wswitch"
		
	}
}

USOFT_ISR_newByte(b)
{
    //isNew = true;
	//myByte = (unsigned char *)b;
	#if USOFT_RXEN
	utb_byteReceived(b);
	#endif
}


UTB_UART_SEND(str)
{
	#if USOFT_TXEN
	usoft_putString(str);
	#endif
}
