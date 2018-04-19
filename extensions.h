/*
* extensions.h
*
* Created: 05-Oct-17 13:13:20
*  Author: yahor.halubchyk
*/

#include <stdint.h>
#include <stdbool.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>

#ifndef EXTENSIONS_H_
#define EXTENSIONS_H_

#define _xstr(s) #s
#define valueFromDef(s) _xstr(s)

#define glue(a,b) a##b

//
//**************************** IO
//

//#define SetBit(port,bit) port|=_BV(bit)
//#define ResetBit(port,bit) port&=~_BV(bit)
//#define ToggleBit(port,bit) port^=_BV(bit)

#define io_setPortBit(port, bit) port|=_BV(bit)
#define io_resetPortBit(port, bit) port&=~_BV(bit)
#define io_togglePortBit(port, bit) port^=_BV(bit)
#define io_getPortBit(pin, bitNumber) (pin & (1 << bitNumber))

#define io_set_(what, portName, portNumBit) (glue(what, portName) |= (1 << (portNumBit)))
#define io_reset_(what, portName, portNumBit) (glue(what, portName) &= ~(1 << (portNumBit)))
#define io_toggle_(what, portName, portNumBit) (glue(what, portName) ^= (1 << (portNumBit)))
#define io_change_(what, portName, portNumBit, bitValue) (glue(what, portName) = ( (glue(what, portName) & ~(1 << (portNumBit)) ) | ((bitValue ? 1: 0) << (portNumBit)) ))
#define io_get_(what, portName, portNumBit) (glue(what, portName) & (1 << (portNumBit)))

#define io_set(what, portDef) io_set_(what, portDef)
#define io_reset(what, portDef) io_reset_(what, portDef)
#define io_toggle(what, portDef) io_toggle_(what, portDef)
#define io_change(what, portDef, bitValue) io_change_(what, portDef, bitValue)
#define io_get(what, portDef) io_get_(what, portDef)

#define io_setPort(portDef) io_set_(PORT, portDef)
#define io_resetPort(portDef) io_reset_(PORT, portDef)
#define io_togglePort(portDef) io_toggle_(PORT, portDef)
#define io_changePort(portDef, bitValue) io_change_(PORT, portDef, bitValue)
#define io_getPin(/* PIN, */ portDef) io_get_(PIN, portDef)

#define io_bitNumber_(portName, numBit) numBit
#define io_bitNumber(portBit) io_bitNumber_(portBit)

#define io_waitPin__(what, portName, portNumBit, isHigh, usCount) _io_waitPin(&glue(what, portName), portNumBit, isHigh, usCount)
#define io_waitPin_(portName, portNumBit, isHigh, usCount) io_waitPin__(PIN, portName, portNumBit, isHigh, usCount)
#define io_waitPin(portDef, isHigh, usCount) io_waitPin_(portDef, isHigh, usCount)

bool _io_waitPin(volatile uint8_t *portName, uint8_t bitNumber, bool waitedBit, uint16_t count);
															   
//
//************************************* asm
//
#define asm_nop() __asm__ volatile("nop")
#define asm_cli() cli()
#define asm_sei() sei()

//
//************************************ string functions
//
int8_t str_indexOf(unsigned char *str, const unsigned char symb);
int8_t str_lastIndex(unsigned char *str);
unsigned char str_lastValue(unsigned char *str);

void delay_ms(uint16_t t);

//
//************************************ types
//

typedef struct time_s{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t dayWeek;
} time_s;

typedef struct portValue_s{
	uint8_t num;
	uint8_t value;
} portValue_s;

#include "extensions.c"
#endif /* EXTENSIONS_H_ */