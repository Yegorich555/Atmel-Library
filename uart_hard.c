/*
* uart_hard.c
*
* Created: 19-Sep-17 14:03:14
*  Author: yahor.halubchyk
*/
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include "uart_hard.h"

#define _uhard_putChar(b) loop_until_bit_is_set(UCSRA, UDRE); UDR = b;

void uhard_putChar(unsigned char b)
{
	_uhard_putChar(b);
}
void uhard_putCharf(const char b)
{
	_uhard_putChar(b);
}

void uhard_putString(unsigned char *str)
{
	while(*str)
	{
		_uhard_putChar(*str);
		++str;
	}
}

void uhard_putStringf(const char *str)
{
	while(*str)
	{
		_uhard_putChar(*str++);
	}
}

void uhard_putBytes(unsigned char *str, uint8_t num)
{
	while(num)
	{
		_uhard_putChar(*str++);
		--num;
	}
}


void uhard_init(void)
{

	//use precision
	#if UHARD_USE_2X
	UCSRA |=  1 << U2X;
	#endif

	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;

	// enable RX and TX and set interrupts on rx complete
	#if UHARD_TXEN == 1
	UCSRB =  1 << TXEN;
	#endif

	#if UHARD_RXEN == 1
	UCSRB |= (1 << RXCIE) | (1 << RXEN);
	#endif

	//this some error UCSRC = 1 << URSEL; // use 	UCSRA instead of UCSRB for read status receiving

	#if defined(UHARD_SYNCMODE) && UHARD_SYNCMODE == 1
	UCSRC |= 1 << UMSEL;
	// (0<<UCPOL);
	#endif
	
	#if UHARD_PARITY == 1 //Parity: Even
	UCSRC |= 1 << UPM1;
	#elif UHARD_PARITY == 2 //Parity: Odd
	UCSRC |= 1 << UPM0;
	#endif

	#if UHARD_STOPBITS == 2
	UCSRC |  1 << USBS;
	#endif

	#if UHARD_BITSIZE == 6
	UCSRC |= 1 << UCSZ0;
	#elif UHARD_BITSIZE == 7
	UCSRC |= 1 << UCSZ1;
	#elif UHARD_BITSIZE == 8
	UCSRC |= (1 << UCSZ1) | (1 << UCSZ0);
	#endif

	//8-bit, 1 stop bit, no parity, asynchronous UART
	UCSRC = (0<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (0<<UCSZ1) | (0<<UCSZ0) | (0<<UCPOL);
}

#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)
#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)

#if UHARD_RXEN
ISR(USART_RXC_vect)
{
	uint8_t data = UDR;

	//uint8_t status = UCSRA;
	//if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	//{
	//
	//}
	__uhard_newByte(data);
}
#endif