/*
* uart_hard.h
*
* Created: 19-Sep-17
*  Author: yahor.halubchyk
*/
#include "extensions.h"

#ifndef UART_HARD_H_
#define UART_HARD_H_

#ifndef UHARD_BUFFER_EN	//set false if need work without buffer...method  uart_GetChar() doesn't work without buffer;
#pragma message "set default => UHARD_bufferEnable = false"
#define UHARD_BUFFER_EN false
#endif

#ifndef UHARD_TXEN
#define UHARD_TXEN true
#endif

#ifndef UHARD_RXEN
#define UHARD_RXEN true
#endif

#ifndef F_CPU
#warning 'F_CPU' not defined
#define F_CPU 1000000
#endif 

#ifndef UHARD_BAUD
#pragma message "set default => UHARD_BAUD = 9600"
#define UHARD_BAUD 9600
#endif

#ifndef UHARD_BAUD_TOL //baud tolerance: default is 2%
#define UHARD_BAUD_TOL 2
#endif

#ifndef UHARD_SYNCMODE
#define UHARD_SYNCMODE 0
#endif

#ifndef UHARD_PARITY   //Parity: 0 - None, 1 - Even, 2 - Odd
#define UHARD_PARITY 0
#endif

#ifndef UHARD_STOPBITS // 1 or 2
#define UHARD_STOPBITS 1
#endif

#ifndef UHARD_BITSIZE //5, 6, 7 or 8 (maybe 9 see datasheet.pdf)
#define UHARD_BITSIZE 8
#endif


/* 
* Calc BaudRate 
*/
#ifdef __ASSEMBLER__
	#define UBRR_VALUE (((F_CPU) + 8 * (UHARD_BAUD)) / (16 * (UHARD_BAUD)) -1)
#else
	#define UBRR_VALUE (((F_CPU) + 8UL * (UHARD_BAUD)) / (16UL * (UHARD_BAUD)) -1UL)
#endif

#if 100 * (F_CPU)	>	 (16 * ((UBRR_VALUE) + 1)) * (100 * (UHARD_BAUD) + (UHARD_BAUD) * (UHARD_BAUD_TOL))
	#define UHARD_USE_2X 1
#elif 100 * (F_CPU)		<		(16 * ((UBRR_VALUE) + 1)) * (100 * (UHARD_BAUD) - (UHARD_BAUD) * (UHARD_BAUD_TOL))
	#define UHARD_USE_2X 1
#else
	#define UHARD_USE_2X 0
#endif

#if UHARD_USE_2X
	#undef UBRR_VALUE
	#ifdef __ASSEMBLER__
		#define UBRR_VALUE (((F_CPU) + 4 * (UHARD_BAUD)) / (8 * (UHARD_BAUD)) -1)
	#else
		#define UBRR_VALUE (((F_CPU) + 4UL * (UHARD_BAUD)) / (8UL * (UHARD_BAUD)) -1UL)
	#endif

	#if 100 * (F_CPU)	 >		(8 * ((UBRR_VALUE) + 1)) * (100 * (UHARD_BAUD) + (UHARD_BAUD) * (UHARD_BAUD_TOL))
	#  warning "Baud rate achieved is higher than allowed"
	#endif

	#if 100 * (F_CPU) < (8 * ((UBRR_VALUE) + 1)) * (100 * (UHARD_BAUD) - (UHARD_BAUD) * (UHARD_BAUD_TOL))
	#  warning "Baud rate achieved is lower than allowed"
	#endif
#endif /* USE_U2X */

#ifdef UBRR_VALUE
		/* Check for overflow */
	#if UBRR_VALUE >= (1 << 12)
	#   warning "UBRR value overflow"
	#endif

	#define UBRRL_VALUE (UBRR_VALUE & 0xff)
	#define UBRRH_VALUE (UBRR_VALUE >> 8)		
#endif

/*
* Calc BaudRate
*/

void uhard_init(void);

/******************** Rx config ********************/
#if UHARD_RXEN
	#if UHARD_BUFFER_EN
		#ifndef UHARD_BUFFER_SIZE
			#warning 'UHARD_BUFFER_SIZE' is not defined
			#define UHARD_BUFFER_SIZE 15
		#endif

		unsigned char uhard_getChar(void);
	#endif

	void __uhard_newByte(uint8_t b);
	#define UHARD_ISR_newByte(b) void __uhard_newByte(uint8_t b)
#endif

/******************** Tx config ********************/
#if UHARD_TXEN
	void uhard_putChar(unsigned char b);
	void uhard_putCharf(const char b);
	void uhard_putString(unsigned char *str);
	void uhard_putStringf(const char *str);
	void uhard_putBytes(unsigned char *str, uint8_t num);
#endif

#include "uart_hard.c"
#endif /* UART_HARD_H_ */