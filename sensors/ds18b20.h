 /* temperature sensor
 * ds18b20.h
 *
 * Created: 30-Oct-17
 *  Author: yahor.halubchyk
 */
 #include <stdint.h>
 #include <stdbool.h>

 #ifndef DS18B20_H_
 #define DS18B20_H_

 #ifndef SENS_DS18B20_NUM
 #pragma message "set default => SENS_DS18B20_NUM = 1"
 #define SENS_DS18B20_NUM 1
 #endif

 #ifndef SENS_DS18B20_IO
 #pragma message "set default => SENS_DS18B20_IO = A, 0"
 #define SENS_DS18B20_IO A, 0
 #endif

 //bool sens_ds18b20_init();
 //void sens_ds1820_read();
 bool sens_ds1820_readByRom(int16_t *t_values, uint8_t romCodes[][8]);
 #include "ds18b20.c"
 #endif /* DS18B20_H_ */
