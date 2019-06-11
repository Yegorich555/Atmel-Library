/*
* SoftUart_Attiny13.c
*
* Created: 18-Apr-18 10:33:47
* Author : yahor.halubchyk
*/

#define F_CPU 9600UL*1000
#include <avr/io.h>
#include <extensions.h>

//************** soft uart
//#define USOFT_IO_MEANDR B, 1

#define USOFT_BAUD 4800
#define USOFT_IO_RX B, 4
#define USOFT_IO_TX B, 3
#define USOFT_TXEN true
#define USOFT_BUFFER_EN true
#define USOFT_BUFFER_SIZE 14
#define USOFT_AUTOLISTEN true
#include <uart_soft.h>

unsigned char myByte;
int main(void)
{
	usoft_init();
	usoft_putStringf("Test\r\n");
	
	//test TX
	while(1)
	{
		//while(usoft_rxCounter<3)
		//{
		//usoft_listen();
		//}

		if (!usoft_rx_work && usoft_rxCounter)
		{
			delay_ms(50);
			if (!usoft_rx_work && usoft_rxCounter)
			{
				for (uint8_t i=0;i<usoft_rxCounter;++i)
				{
					usoft_putChar(usoft_rxBuffer[i]);
				}
				usoft_rxCounter = 0;
			}
		}
	}
}

USOFT_ISR_newByte(b)
{
	myByte = b;
}

//ISR(PCINT0_vect)
//{
//usoft_listen();
//}