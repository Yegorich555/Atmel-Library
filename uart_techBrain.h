/*
* uart_techBrain.h
*
* Created: 15-Sep-17 17:41:33
*  Author: yahor.halubchyk
*/

#include <stdbool.h>

#ifndef UART_TECHBRAIN_H_
#define UART_TECHBRAIN_H_

#ifndef	UTB_ADDING_BUFFER
#define UTB_ADDING_BUFFER false 
#endif

#ifndef	UTB_REPEATER_ONLY
#define UTB_REPEATER_ONLY false
#endif

#ifndef	UTB_BUFFER_SIZE
  #if FLASHEND <= 1024
	 #define UTB_BUFFER_SIZE 15
  #else
	#define UTB_BUFFER_SIZE 30
  #endif
#endif

#define utb_startByte '>' //62
#define utb_cmdByte '^' //94
#define utb_endByte  250

#define utb_commonAnswerAddr 98
#define utb_commonAddr 99
#define utb_defaultAddr 97

#define utb_receivingMinSize 6
unsigned char utb_buffer[UTB_BUFFER_SIZE + 1];

#if UTB_ADDING_BUFFER
unsigned char utb_rx_buffer[UTB_BUFFER_SIZE + 1];
#endif

typedef enum utb_cmd_e {
	none = 0,
	set_address,
	get_address,
	toggle_Repeater,
	set_out1,
	reset_out1,
	set_out2,
	reset_out2,
	set_outs,
	reset_outs,
	set_clock,
	get_sensors,
	change_outs,  //only for FLASHEND > 1024
	change_out
} utb_cmd_e;

void utb_init();

/* Init with send address pointer
*  utb_init2(&time, &portValue, arraySensorsValue)	
*/
void utb_init2(time_s *t, portValue_s *pv, int16_t *sensorsValue[], uint8_t arrSensSize);

void __utb_uart_send(unsigned char *str, uint8_t num) __attribute__((weak));;
#define UTB_UART_SEND(str, num) void __utb_uart_send(unsigned char *str, uint8_t num)

void utb_byteReceived(unsigned char b);
utb_cmd_e utb_getCmd();

#include "uart_techBrain.c"
#endif /* UART_TECHBRAIN_H_ */