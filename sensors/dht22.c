
#include "extensions.h"
#include "dht22.h"

const char dTroom = 5; // Коррекция температуры

bool sens_dht22_waitPin(bool waitedBit)
{
	bool good = io_waitPin(SENS_DHT22_IO, waitedBit, 10000);
	return good;
}

bool sens_dht22_read(int16_t *hum_value, int16_t *t_value)
{
	unsigned char n, k, idx;
	unsigned char data[5];
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	io_set(DDR, SENS_DHT22_IO);
	io_setPort(SENS_DHT22_IO);
	delay_ms(200);
	io_resetPort(SENS_DHT22_IO);
	delay_ms(18);
	io_setPort(SENS_DHT22_IO);
	io_reset(DDR, SENS_DHT22_IO);
	_delay_us(50);
	if(io_getPin(SENS_DHT22_IO) != 0) return false;

	if (!sens_dht22_waitPin(1)) return false;
	if (!sens_dht22_waitPin(0)) return false;

	for(n = 0; n < 5; n++)
	{
		idx = 0x80;
		for(k = 0; k < 8; k++)
		{
			if (!sens_dht22_waitPin(1)) return false;
			_delay_us(33);
			if(io_getPin(SENS_DHT22_IO) != 0) data[n] |= idx;
			idx >>= 1;
			if (!sens_dht22_waitPin(0)) return false;
		}
	}
	*hum_value = data[0] * 256 + data[1];
	*t_value = data[2] * 256 + data[3] + dTroom;

	return true;
}