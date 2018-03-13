/*
* extensions.c
*
* Created: 05-Oct-17 13:12:45
*  Author: yahor.halubchyk
*/
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include "extensions.h"

bool _io_waitPin(volatile uint8_t *portName, uint8_t bitNumber, bool waitedBit, uint16_t usCount)
{
	uint16_t cnt = 0;
	uint8_t mask;
	if (waitedBit) mask = 1 << bitNumber;
	else mask = 0;
	
	while(io_getPortBit(*portName, bitNumber) ^ mask)
	{
		++cnt;
		if (cnt >= usCount) return false;
		_delay_us(1);
	}
	return true;
}

int8_t str_indexOf(unsigned char *str, const unsigned char symb)
{
	int8_t index = 0;
	while(*str)	 //check forward
	{
		if (*str == symb)
		return index;
		++str;
		++index;
	}
	return -1;
}

int8_t str_lastIndex(unsigned char *str)
{
	int8_t index = -1;
	while(*str)
	{
		++str;
		++index;
	}
	return index;
}

unsigned char str_lastValue(unsigned char *str)
{
	while(*(str + 1))
	{
		++str;
	}
	return *str;
}

void delay_ms(uint16_t t)
{
	for (uint16_t n = 0; n < t; ++n)
	_delay_us(1000);
};