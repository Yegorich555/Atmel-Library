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
#define USOFT_BUFFER_SIZE 4
#define USOFT_AUTOLISTEN true
#include <uart_soft.h>

unsigned char myByte;
int main(void)
{
	usoft_init();
	
	//MCUCR &= ~(1<<ISC01) | ~(0<<ISC00);	// Trigger INT0 on rising edge
	//PCMSK |= (1<<PCINT4);   // pin change mask: listen to portb, pin PB3
	//GIMSK |= (1<<PCIE); // enable PCINT interrupt

	//test RX
	while(1)
	{
		usoft_putStringf("test uart: "); 	usoft_putCharf(0x0D);
		usoft_putUInt(0); 		usoft_putCharf(0x0D);
		usoft_putUInt(10); 		usoft_putCharf(0x0D);
		usoft_putUInt(100); 	usoft_putCharf(0x0D);
		usoft_putUInt(101); 	usoft_putCharf(0x0D);
		usoft_putUInt(1000); 	usoft_putCharf(0x0D);
		usoft_putUInt(10000); 	usoft_putCharf(0x0D);
		usoft_putUInt(65432); 	usoft_putCharf(0x0D);
		
		delay_ms(500);
		break;
	}

	//test TX
	while(1)
	{
		//while(usoft_rxCounter<3)
		//{
		//usoft_listen();
		//}

		if (!usoft_rx_work && usoft_rxCounter)
		{
			delay_ms(500);
			usoft_putChar(usoft_rxBuffer[0]);
			usoft_putChar(usoft_rxBuffer[1]);
			usoft_putChar(usoft_rxBuffer[2]);
			usoft_putChar(usoft_rxCounter);
			usoft_rxCounter = 0;
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