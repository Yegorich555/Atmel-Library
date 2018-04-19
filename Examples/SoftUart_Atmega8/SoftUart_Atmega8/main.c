/*
 * SoftUart_Atmega8.c
 *
 * Created: 19-Apr-18 17:11:30
 * Author : yahor.halubchyk
 */ 

#define F_CPU 8000UL*1000
#include <avr/io.h>
#include <extensions.h>
#include <avr/interrupt.h>

//************** soft uart
#define USOFT_IO_MEANDR D, 4

#define USOFT_BAUD 4800
#define USOFT_IO_RX D, 3
#define USOFT_IO_TX D, 2
#define USOFT_TXEN true
#define USOFT_RXEN true
#define USOFT_BUFFER_EN true
#define USOFT_BUFFER_SIZE 4
//#define USOFT_AUTOLISTEN true
#include <uart_soft.h>

unsigned char myByte;

int main(void)
{
	usoft_init();
	
	//test RX
	while(1)
	{
		usoft_putStringf("test uart");
		usoft_putCharf(0x0D); //end line
		delay_ms(500);
		break;
	}

	//test TX
	while(1)
	{
		while(usoft_rxCounter<3)
		{
			usoft_listen();
		}

		if (!usoft_rx_work && usoft_rxCounter)
		{
			delay_ms(500);
			usoft_putChar(usoft_rxBuffer[0]);
			usoft_putChar(usoft_rxBuffer[1]);
			usoft_putChar(usoft_rxBuffer[2]);
			usoft_putChar(usoft_rxCounter);
			usoft_putChar(myByte);
			usoft_rxCounter = 0;
		}
	}
}

USOFT_ISR_newByte(b)
{
	myByte = b;
}