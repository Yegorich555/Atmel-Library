 /*	Clock
 * dht22.h
 *
 * Created: 30-Oct-17
 *  Author: yahor.halubchyk
 */
 #include <stdint.h>
 #include <stdbool.h>
 #include "extensions.h"

 #ifndef DS1307_H_
 #define DS1307_H_

 #ifndef SENS_DS1307_IO_SCL
 #pragma message "set default => SENS_DS1307_IO_SCL = B, 0"
 #define SENS_DS1307_IO_SCL B, 0
 #endif

 #ifndef SENS_DS1307_IO_SDA
 #pragma message "set default => SENS_DS1307_IO_SDA = B, 1"
 #define SENS_DS1307_IO_SDA B, 1
 #endif

 #define I2C_SOFT_IO_SCL SENS_DS1307_IO_SCL
 #define I2C_SOFT_IO_SDA SENS_DS1307_IO_SDA
 
 typedef enum ds1307_addr_e {
	 /* 0..59 */
	 seconds = 0,
	 /* 0..59 */
	 minutes = 1,
	 /* 1..12 AM, 0..23 PM */
	 hours = 2,
	 /* 1..7 */
	 weekDay = 3,
	 /* 1..31 */
	 date = 4,
	 /* 1..12 */
	 month = 5,
	 /* 0..99 */
	 year = 6,
	 /* control sqwe, rs1, rs0 */
	 control = 7
 } ds1307_addr_e;

 bool sens_ds1307_init();
 bool sens_ds1307_getTime(time_s *t);
 bool sens_ds1307_setTime(time_s t);
 uint8_t sens_ds1307_read(ds1307_addr_e adr);
 bool sens_ds1307_write(ds1307_addr_e adr, uint8_t val);

 #include "ds1307.c"
 #endif /* DS1307_H_ */
