/*
* i2c_soft.c
*
* Created: 31-Oct-17
*  Author: yahor.halubchyk
*/

#include "extensions.h"
#include "i2c_soft.h"

#include <util/delay.h>
#define _i2c_short_delay() _delay_loop_2(3)
#define _i2c_long_delay() _delay_loop_2(5)

#define _i2c_SCL_low()	 io_set(DDR, I2C_SOFT_IO_SCL)
#define _i2c_SCL_high()	 io_reset(DDR, I2C_SOFT_IO_SCL)
#define _i2c_SDA_low()   io_set(DDR, I2C_SOFT_IO_SDA)
#define _i2c_SDA_high()  io_reset(DDR, I2C_SOFT_IO_SDA)

void i2c_soft_init()
{
	io_resetPort(I2C_SOFT_IO_SCL);
	io_resetPort(I2C_SOFT_IO_SDA);

	_i2c_SDA_low();
	_i2c_SCL_low();
	
	for(uint8_t i = 0; i < 10; ++i)
	{
		_i2c_SCL_high(); _i2c_long_delay();
		_i2c_SCL_low(); _i2c_long_delay();
	}
	_i2c_SDA_high();
	_i2c_SCL_high();
}

void i2c_soft_start(void)
{
	//_i2c_SCL_high(); _i2c_long_delay();
	//_i2c_SDA_low(); _i2c_long_delay();

	_i2c_SDA_high(); _i2c_long_delay();
	_i2c_SCL_high(); _i2c_long_delay();

	_i2c_SDA_low(); _i2c_long_delay();
	_i2c_SCL_low(); _i2c_long_delay();
}

void i2c_soft_stop(void)
{
	_i2c_SDA_low(); _i2c_long_delay();
	_i2c_SCL_high(); _i2c_short_delay();
	_i2c_SDA_high(); _i2c_long_delay();
}

static bool _waitSCL()
{
     return io_waitPin(I2C_SOFT_IO_SCL, 1, 10);
}

bool i2c_soft_write(uint8_t b)
{
	uint8_t i;
	for(i = 0; i < 8; ++i)
	{
		_i2c_SCL_low(); _i2c_short_delay();

		if(b & 0x80) _i2c_SDA_high();
		else _i2c_SDA_low();
		_i2c_long_delay();

		_i2c_SCL_high(); _i2c_long_delay();
		if (!_waitSCL())	return false;
		b = b << 1;
	}

	_i2c_SCL_low(); _i2c_short_delay();
	_i2c_SDA_high(); _i2c_long_delay();
	_i2c_SCL_high(); _i2c_long_delay();

	bool ack = !io_getPin(I2C_SOFT_IO_SDA);
	_i2c_SCL_low(); _i2c_long_delay();
	return ack;
}

uint8_t i2c_soft_read(bool ack)
{
	uint8_t data = 0;
	for(uint8_t i = 0; i < 8; ++i)
	{
		_i2c_SCL_low(); _i2c_long_delay();
		_i2c_SCL_high(); _i2c_long_delay();
		if (!_waitSCL()) return 0;
		if(io_getPin(I2C_SOFT_IO_SDA))
		{
			data |= ( 0x80 >> i );
		}
	}
	_i2c_SCL_low(); _i2c_long_delay();

	if(ack)	_i2c_SDA_low();
	else _i2c_SDA_high();
	_i2c_long_delay();

	_i2c_SCL_high(); _i2c_long_delay();
	_i2c_SCL_low();
	_i2c_SDA_high(); _i2c_long_delay();

	return data;
}


