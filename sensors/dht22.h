 /*	Humidity sensor
 * dht22.h
 * 
 * Created: 30-Oct-17
 *  Author: yahor.halubchyk
 */
 #include <stdint.h>
 #include <stdbool.h>

 #ifndef DHT22_H_
 #define DHT22_H_

 #ifndef SENS_DHT22_IO
 #pragma message "set default => SENS_DHT22_IO = A, 0"
 #define SENS_DHT22_IO A, 0
 #endif

 //bool sens_dht22_init();
 bool sens_dht22_read(int16_t *hum_value, int16_t *t_value);

 #include "dht22.c"
 #endif /* DHT22_H_ */
