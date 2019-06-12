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
#define USOFT_IO_MEANDER B, 3

#define USOFT_BAUD 4800
#define USOFT_IO_RX IO_DEBUG_RX
#define USOFT_IO_TX IO_DEBUG_TX
#define USOFT_TXEN true
#define USOFT_BUFFER_EN true
#define USOFT_BUFFER_SIZE 14
//#define USOFT_AUTOLISTEN true
#define USOFT_NewByteEvent_EN 1
#include <uart_soft.h>

unsigned char myByte;
bool isEnd;
int main(void)
{
	//MCUCR &= ~(1<<ISC01) | ~(0<<ISC00);	// Trigger INT0 on rising edge
	//PCMSK |= (1<<PCINT4);   // pin change mask: listen to portb, pin PB3
	//GIMSK |= (1<<PCIE); // enable PCINT interrupt
	
	usoft_init();
	usoft_putStringf("Test\r\n");
	
	//test RX
	//while(1) // for #define USOFT_AUTOLISTEN true
	//{
		//if (!usoft_rx_work && usoft_rxCounter)
		//{
			//delay_ms(10);
			//if (!usoft_rx_work && usoft_rxCounter)
			//{
				//for (uint8_t i=0;i<usoft_rxCounter;++i)
				//{
					//usoft_putChar(usoft_rxBuffer[i]);
				//}
				//if (myByte)
				//{
					//usoft_putChar(myByte);
				//}
				//usoft_rxCounter = 0;
			//}
		//}
	//}
	
	while(1) // for #define USOFT_AUTOLISTEN false
	{
		usoft_listen();
		if (isEnd)
		{
			isEnd = false;
			delay_ms(10);
			for (uint8_t i=0;i<usoft_rxCounter;++i)
			{
				usoft_putChar(usoft_rxBuffer[i]);
			}
			if (myByte)
			{
				usoft_putChar(myByte);
			}
			usoft_rxCounter = 0;
		}
	}
}

USOFT_ISR_newByte(b)
{
	myByte = b;
	if (b=='l'){
		isEnd = true;
	}
}
//
//ISR(PCINT0_vect)
//{
	//usoft_listen();
//}