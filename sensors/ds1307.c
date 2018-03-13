#include "ds1307.h"
#include "i2c_soft.h"

#define _sens_ds1307_castTo(v) (((v / 10) << 4) | (v % 10))
#define _sens_ds1307_castFrom(v) ((((v & 0xF0) >> 4) * 10) + (v & 0x0F))

#define _SENS_DS1307_ADR 0x68
#define _SENS_DS1307_WRITE  0xD0
#define _SENS_DS1307_READ   0xD1

bool sens_ds1307_init()
{
	i2c_soft_init();                             // Initialize the I2c module.
	i2c_soft_start();                            // Start I2C communication
	if (!i2c_soft_write(_SENS_DS1307_WRITE)) return false;        // Connect to DS1307 by sending its ID on I2c Bus
	if (!i2c_soft_write(control)) return false;			// Select the Ds1307 ControlRegister to configure Ds1307
	if (!i2c_soft_write(0x00)) return false;                        // Write 0x00 to Control register to disable SQW-Out

	i2c_soft_stop();                             // Stop I2C communication after initializing DS1307
	return true;
}

uint8_t sens_ds1307_read(ds1307_addr_e adr)
{
	i2c_soft_start();
	if (!i2c_soft_write(_SENS_DS1307_WRITE)) return 0;
	if (!i2c_soft_write(adr)) return 0;
	i2c_soft_stop();
	i2c_soft_start();
	if (!i2c_soft_write(_SENS_DS1307_READ)) return 0;
	uint8_t value = i2c_soft_read(false);
	i2c_soft_stop();
	return _sens_ds1307_castFrom(value);
}

bool sens_ds1307_write(ds1307_addr_e adr, uint8_t val)
{
	i2c_soft_start();
	if (!i2c_soft_write(_SENS_DS1307_WRITE)) return false;
	if (!i2c_soft_write(adr)) return false;
	if (!i2c_soft_write(_sens_ds1307_castTo(val))) return false;
	i2c_soft_stop();

	return true;
}

bool sens_ds1307_getTime(time_s *t)
{
	i2c_soft_start();
	if (!i2c_soft_write(_SENS_DS1307_WRITE)) return false;
	if (!i2c_soft_write(seconds)) return false;
	i2c_soft_stop();
	i2c_soft_start();
	if (!i2c_soft_write(_SENS_DS1307_READ)) return false;
	uint8_t v = i2c_soft_read(1);	t->sec = _sens_ds1307_castFrom(v);
	v = i2c_soft_read(1);			t->min = _sens_ds1307_castFrom(v);
	v = i2c_soft_read(0);			t->hour = _sens_ds1307_castFrom(v);
	i2c_soft_stop();
	return true;
}

bool sens_ds1307_setTime(time_s t)
{
	i2c_soft_start();
	if (!i2c_soft_write(_SENS_DS1307_WRITE)) return false;
	if (!i2c_soft_write(seconds)) return false;
	if (!i2c_soft_write(_sens_ds1307_castTo(t.sec))) return false;
	if (!i2c_soft_write(_sens_ds1307_castTo(t.min))) return false;
	if (!i2c_soft_write(_sens_ds1307_castTo(t.hour))) return false;
	i2c_soft_stop();
	return true;
}