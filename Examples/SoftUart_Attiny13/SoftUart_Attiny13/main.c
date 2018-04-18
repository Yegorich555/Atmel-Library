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
#define USOFT_BAUD 4800
#define USOFT_IO_RX B, 4
#define USOFT_IO_TX B, 3
#define USOFT_BUFFER_EN false
#include <uart_soft.h>

unsigned char myByte;
bool isNew = false;

int main(void)
{
	usoft_init();

	//test RX
	while(1)
	{
	  usoft_putStringf("test uart");
	  usoft_putCharf(0x0D); //end line
	  delay_ms(500);
	}

	//test TX
	while(1)
	{
		usoft_listen();
		if (isNew)
		{
			usoft_putChar(myByte);
			isNew = false;
		}
	}
}

USOFT_ISR_newByte(b)
{
    isNew = true;
	myByte = b;
}


