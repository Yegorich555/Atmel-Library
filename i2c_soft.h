/*
* i2c_soft.h
*
* Created: 31-Oct-17
*  Author: yahor.halubchyk
*/

#ifndef _I2C_SOFT_H_
#define _I2C_SOFT_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef I2C_SOFT_IO_SCL
#error "I2C_SOFT_IO_SCL didn't define"
#define I2C_SOFT_IO_SCL B, 0
#endif

#ifndef I2C_SOFT_IO_SDA
#error "I2C_SOFT_IO_SDA didn't define"
#define I2C_SOFT_IO_SDA B, 1
#endif

void i2c_soft_init();
void i2c_soft_start();
void i2c_soft_stop();
bool i2c_soft_write(uint8_t b);
uint8_t i2c_soft_read(bool ack);

#include "i2c_soft.c"
#endif /* _I2C_SOFT_H_ */
