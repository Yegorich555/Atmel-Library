/*
 * Attiny13A_Transmitter.c
 *
 * Created: 11.06.2019 9:53:20
 * Author : YegorVM
 */ 

#define F_CPU 9600UL*1000
#include <avr/io.h>

#define USOFT_BAUD 4800
#define USOFT_IO_TX IO_DEBUG_TX
#define USOFT_TXEN true
#define USOFT_RXEN false
#include <uart_soft.h>

int main(void)
{
    usoft_init();
    while (1) 
    {
		delay_ms(200);
		usoft_putStringf("Test_parcel");
    }
}

