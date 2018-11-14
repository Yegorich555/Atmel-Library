/*
* soft_UART.c
*
* Created: 12-Sep-17 17:07:06
*  Author: yahor.halubchyk
*/
#include "extensions.h"
#include <stdint.h>
#include <stdbool.h>

#include "uart_soft.h"

static volatile uint8_t usoft_timeCount;
static volatile unsigned char usoft_rx_tx_byte;
static volatile bool usoft_tx_work;
static volatile bool usoft_rx_work;
static volatile uint8_t usoft_iBit;

#if USOFT_BUFFER_EN
static volatile uint8_t usoft_rxCounter;
static uint8_t usoft_rxBuffer[USOFT_BUFFER_SIZE + 1];
static uint8_t usoft_rxReadIndex;
#endif

#define USOFT_timerStart() USOFT_tCNT = USOFT_tCNTvalue;
//#define timerStart() uart_tCNT=uart_tCNTvalue; uart_tCCR=uart_tCCRvalue
//#define timerStop() uart_tCCR=0;
//#define timerStartGo() uart_tCNT=0xFF;// uart_tCCR=uart_tCCRvalue

#if USOFT_RXEN
#define usoft_getRx() io_getPin(USOFT_IO_RX)
#endif

#if USOFT_TXEN
#define usoft_txReset() io_resetPort(USOFT_IO_TX)
#define usoft_txSet() io_setPort(USOFT_IO_TX)
#endif

void usoft_init()
{
	#if USOFT_TXEN
	//set output
	io_set(DDR, USOFT_IO_TX);
	usoft_txSet();

	#if USOFT_IO_MEANDR
	io_set(DDR, USOFT_IO_MEANDR);
	#endif

	#endif

	#if USOFT_RXEN
	//set input
	io_reset(DDR, USOFT_IO_RX);
	io_resetPort(USOFT_IO_RX);
	#endif

	//timerStop();
	USOFT_tCCR = USOFT_tCCRvalue;
	USOFT_timerStart();
	USOFT_setTIMSK();

	asm_sei();
}

#if USOFT_RXEN
void usoft_listen()
{
	if (!usoft_tx_work && !usoft_rx_work && usoft_getRx() == 0) //start bit
	{
		USOFT_timerStart();
		usoft_rx_work = 1;
		usoft_rx_tx_byte = 0;
		usoft_timeCount = 0;
		usoft_iBit = 0;
	}
}
#endif

static inline void rxReset()	//break off rx
{
	usoft_rx_work = 0;
}

#if USOFT_TXEN
static void setChar(uint8_t b)
{
	usoft_iBit = 0;
	usoft_rx_tx_byte = b;
	usoft_timeCount = 0;
	usoft_tx_work = 1;
	while (usoft_tx_work);
}

void usoft_putChar(uint8_t b)
{
	rxReset();
	setChar(b);
}

void usoft_putCharf(const char b)
{
	rxReset();
	setChar(b);
}

void usoft_putString(uint8_t *str)
{
	rxReset();
	while (*str)
	{
		setChar(*str++);
	}
}

void usoft_putStringf(const char *str)
{
	rxReset();
	while (*str)
	{
		setChar(*str++);
	}
}

void usoft_putBytes(unsigned char *str, uint8_t num)
{
	rxReset();
	while(num)
	{
		setChar(*str++);
		--num;
	}
}

void usoft_putUInt(unsigned int v)
{
	uint8_t go = 0;
	for (unsigned int i = 10000; i >= 1; i = i / 10)
	{
		uint8_t digit = (v/i % 10);
		if (go || digit) {
			usoft_putChar(48 + digit);
			go = 1;
		}
	}
	if (!go)
		usoft_putChar(48 + v);
	//usoft_putChar(48 + (v/10000 % 10));
	//usoft_putChar(48 + (v/1000 % 10));
	//usoft_putChar(48 + (v/100 % 10));
	//usoft_putChar(48 + (v/10 % 10));
	//usoft_putChar(48 + (v/1 % 10));
}

static void txSend()
{
	if (usoft_iBit == 0) //Start bit
	{
		usoft_txReset();
	}
	else if (usoft_iBit < 9)
	{
		if (usoft_rx_tx_byte & ( 1 << (usoft_iBit - 1) ))
		{
			usoft_txSet();
		}
		else
		{
			usoft_txReset();
		}
	}
	else //Stop bit
	{
		usoft_txSet();
	}
	
	if (usoft_iBit == 10) //End parcel action
	{
		usoft_tx_work = 0;
	}

	++usoft_iBit;
}
#endif

#if USOFT_BUFFER_EN
uint8_t usoft_getChar(void)
{
	uint8_t data;
	if (usoft_rxCounter)
	{
		data = usoft_rxBuffer[usoft_rxReadIndex++];
		--usoft_rxCounter;
		if (usoft_rxReadIndex == USOFT_BUFFER_SIZE)
		{
			usoft_rxReadIndex = 0;
		}
		return data;
	}
	else return 0;

	return 0;	 //uartTB_GetByte();
}
#endif

#if USOFT_RXEN
static void rxByteSet(uint8_t b)
{
	usoft_newByte(b);

	#if USOFT_BUFFER_EN
	usoft_rxBuffer[usoft_rxCounter] = b;
	++usoft_rxCounter;
	if (usoft_rxCounter == USOFT_BUFFER_SIZE)
	{
		usoft_rxCounter = 0;
	}

	#endif
}

static void rxReceive()
{
	if (usoft_timeCount < 3)
	{
		return;
	}
	if (usoft_iBit == 8)//todo Ibit
	{
		rxReset();
		if (usoft_getRx()) // successful
		{
			rxByteSet(usoft_rx_tx_byte);
		}
		return;
	}

	if (usoft_getRx())
	{
		usoft_rx_tx_byte |= (1 << usoft_iBit);
	}
	++usoft_iBit;
}
#endif

// Timer 0 overflow interrupt service routine
#ifdef USOFT_tISR
ISR(USOFT_tISR)
{
	USOFT_tCNT = USOFT_tCNTvalue;

	#if USOFT_AUTOLISTEN
	usoft_listen();
	#endif
	
	#if USOFT_IO_MEANDR
	io_togglePort(USOFT_IO_MEANDR);
	#endif

	++usoft_timeCount;
	if (!(usoft_timeCount & 1)) // set mask 1 3 5 7....
	{
		return;
	}

	if (usoft_tx_work)
	{
		#if USOFT_TXEN
		txSend();
		#endif
	}
	else if (usoft_rx_work)
	{
		#if USOFT_RXEN
		rxReceive();
		#endif
	}
}
#endif

