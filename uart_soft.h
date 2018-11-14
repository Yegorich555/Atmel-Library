 /*
 * soft_UART.h
 *
 * Created: 12-Sep-17 17:25:02
 *  Author: yahor.halubchyk
 */
/* example config
 
#define F_CPU 8000UL*1000
#define USOFT_BAUD 9600
#define USOFT_RXEN true
	#define USOFT_RxPortMarker 'B'
	#define USOFT_RxPortNumber 2
#define USOFT_TXEN false
	//#define USOFT_TxPortMarker 'B'
	//#define USOFT_TxPortNumber 3

*/

#include <stdbool.h>

#ifndef UART_SOFT_H_
#define UART_SOFT_H_

#define str(s) xstr(s)
#define xstr(s) #s

#ifndef USOFT_IO_MEANDR
#define USOFT_IO_MEANDR false
#endif

#ifndef USOFT_TXEN
#define USOFT_TXEN true
#endif 

#ifndef USOFT_RXEN
#define USOFT_RXEN true
#endif

#if USOFT_RXEN
	#ifndef USOFT_BUFFER_EN	//set false if need work without buffer...method  uart_GetChar() doesn't work without buffer;
	#pragma message "set default => USOFT_bufferEnable = false"
	#define USOFT_BUFFER_EN false
	#endif
#endif

#ifndef USOFT_AUTOLISTEN
#define USOFT_AUTOLISTEN false
#endif

#ifndef F_CPU
#warning 'F_CPU' not defined
#define F_CPU 1000000
#endif //

#ifndef USOFT_BAUD
#pragma message "set default => USOFT_BAUD = 4800"
#define USOFT_BAUD 4800
#endif //

// only 8 None 1

//t, ms interrupt = 1/baud*2*1000; it's 0.1041666 for 4800 (*2 because twice higher frequency)
#if defined(__AVR_ATtiny13A__)// || defined(__AVR_ATtiny13__)
 	#define USOFT_setTIMSK() TIMSK0|= (1<<TOIE0) //TIMSK0|= (1<<TOIE0); //Enable inerrupt by timer0
 	#define USOFT_tISR TIM0_OVF_vect //timer interrupt vector
 	#define USOFT_tCCR TCCR0B //timer config register
	#define USOFT_tCNT TCNT0	//timer value register

	#if F_CPU == 9600000 && USOFT_BAUD == 4800
	    #define USOFT_tCCRvalue (0<<CS02) | (1<<CS01) | (0<<CS00) //1200 kHz timer config value
	    #define USOFT_tCNTvalue 0x83 + 3 //timer start value
	#endif
#elif defined (__AVR_ATmega16A__) || defined (__AVR_ATmega8A__)
 	#define USOFT_setTIMSK() TIMSK|= (1<<TOIE0) //TIMSK0|= (1<<TOIE0); //Enable inerrupt by timer0
 	#define USOFT_tISR TIMER0_OVF_vect //timer interrupt vector
 	#define USOFT_tCCR TCCR0 //timer config register
 	#define USOFT_tCNT TCNT0	//timer value register

 	#if F_CPU == 8000000 && USOFT_BAUD == 4800
 		#define USOFT_tCCRvalue (0<<CS02) | (1<<CS01) | (0<<CS00)//1000 kHz timer config value	 
		#if defined (__AVR_ATmega16A__)
		 	#define USOFT_tCNTvalue 0x98 //timer start valu
		 #else
			 #define USOFT_tCNTvalue 0x98 + 3//timer start value
		#endif
 	#endif
#else
	#warning Soft_uart with this AVR is not defined, see me *.h
	#define USOFT_setTIMSK() //TIMSK0|= (1<<TOIE0); //Enable inerrupt by timer0
	#define USOFT_tCCR PORTA //timer config register
	#define USOFT_tCNT PORTA //timer value register
#endif

#ifndef USOFT_tCCRvalue
	#warning Soft_uart with currently F_CPU and USOFT_BAUD is not defined for this Chip
	#pragma message "F_CPU = " str(F_CPU) "; USOFT_BAUD = " str(USOFT_BAUD)
#endif

#ifndef USOFT_tCCRvalue
#define USOFT_tCCRvalue 0
#endif

#ifndef USOFT_tCNTvalue
#define USOFT_tCNTvalue 0
#endif

void usoft_init(void);
void usoft_listen(void);


/******************** Rx config ********************/
#if USOFT_RXEN
	#ifndef USOFT_IO_RX
	#warning "USOFT_IO_RX is not defined"
	#define USOFT_IO_RX B, 0
	#endif

   #if USOFT_BUFFER_EN
	   #ifndef USOFT_BUFFER_SIZE
		   #warning 'USOFT_BUFFER_SIZE' not defined
		   #define USOFT_BUFFER_SIZE 15
	   #endif

	    unsigned char usoft_getChar(void);
	#endif

	void usoft_newByte(uint8_t b) __attribute__((weak));
	#define USOFT_ISR_newByte(b) void usoft_newByte(uint8_t b)
#endif


/******************** Tx config ********************/
#if USOFT_TXEN
	#ifndef USOFT_IO_TX
	#warning "USOFT_IO_TX is not defined"
	#define USOFT_IO_TX B, 1
	#endif

	void usoft_putUInt(unsigned int v);
	void usoft_putChar(unsigned char b);
	void usoft_putCharf(const char b);
	void usoft_putString(unsigned char *str);
	void usoft_putStringf(const char *str);
	void usoft_putBytes(unsigned char *str, uint8_t num);
#endif

#include "uart_soft.c"
#endif /* SOFT_UART_H_ */