/*
* uart_techBrain.c
*
* Created: 15-Sep-17 17:38:29
*  Author: yahor.halubchyk
version 1.0
*/
#include <avr/eeprom.h>
#include <stdint.h>
#include <stdbool.h>
#include "uart_techBrain.h"
#include "extensions.h"

static uint8_t EEMEM _e_ownAddress = utb_defaultAddr;
static unsigned char _ownAddress;

static uint8_t _txAnswerEn = 1;
static uint8_t _txRepeatCount = 1;

#if FLASHEND > 1024
static uint8_t EEMEM _e_imRepeater = false;
static bool utb_iRepeater;
#else
#define utb_iRepeater false
#endif

static volatile bool utb_isNewLine = 0;
static volatile int8_t utb_rx_counter = -1;
static volatile int8_t utb_buf_counter = -1;

#if FLASHEND > 1024
static time_s *utb_time;
static portValue_s *utb_portValue;
static int16_t **utb_sensorsValue;
static uint8_t utb_sensorsSize;
#endif

void utb_init()
{
	uint8_t addr = eeprom_read_byte(&_e_ownAddress);
	if (addr != 0xFF) _ownAddress = addr;
	else _ownAddress = utb_defaultAddr;

	#if FLASHEND > 1024
	uint8_t rptr = eeprom_read_byte(&_e_imRepeater);
	if (rptr != 0xFF) utb_iRepeater = rptr;
	else utb_iRepeater = false;
	#endif
}

void utb_init2(time_s *t, portValue_s *pv, int16_t *sensorsValue[], uint8_t arrSensSize)
{
	utb_time = t;
	utb_portValue = pv;
	utb_sensorsSize = arrSensSize;
	utb_sensorsValue = sensorsValue;
	utb_init();
}

static inline bool _checkNewLine()
{
	bool isNew = utb_isNewLine;
	utb_isNewLine = 0;
	return isNew;
}

static inline void _clearLine()
{
	utb_rx_counter = -1;
}

static void _setLine()
{
	#if UTB_ADDING_BUFFER
	memcpy(utb_buffer, utb_rx_buffer, utb_rx_counter);
	#endif
	utb_buf_counter = utb_rx_counter;
	_clearLine();
	utb_isNewLine = 1;
}

void utb_byteReceived(unsigned char b)
{
	if (utb_rx_counter == -1) //wait start char
	{
		if (b == utb_startByte)
		{
			++utb_rx_counter;
		}
		return;
	}
	
	if (b == utb_endByte)  //end line
	{
		if (utb_rx_counter >= utb_receivingMinSize)
		{
			_setLine();
		}
		else
		{
			_clearLine();
		}
	}
	else
	{
		#if UTB_ADDING_BUFFER
		utb_rx_buffer[utb_rx_counter] = b;
		#else
		utb_buffer[utb_rx_counter] = b;
		#endif
		++utb_rx_counter;
		if (utb_rx_counter == UTB_BUFFER_SIZE) _clearLine();
	}
	
}

static uint8_t _getCrc(unsigned char *str, uint8_t num)
{
	uint16_t crc = 0;
	while(num)
	{
		crc = (crc << 3) + *str;
		crc = (crc << 3) + *str;
		crc = crc ^ (crc >> 8);
		++str;
		--num;
	}
	crc = (crc & 0xFF);
	if (crc == utb_endByte)
	{
		--crc;
	}
	return crc;
}

static void _sendRepeat(unsigned char *str)
{
	unsigned char myStr[2] = {utb_startByte, _ownAddress};

	for (int i = 0; i < _txRepeatCount; ++i)
	{
		__utb_uart_send(myStr, 2);
		__utb_uart_send(str, str_lastIndex(str) + 1);
	}
}

static void _sendAnswer(unsigned char *str, uint8_t numBytes)
{
	if (_txRepeatCount < 1)	return;

	unsigned char *buf = utb_buffer;
	*buf = utb_startByte;
	*(++buf) = 0; // it's crc
	unsigned char *first = buf;
	*(++buf) = _ownAddress;
	*(++buf) = utb_cmdByte;
	*(++buf) = utb_commonAnswerAddr;
	*(++buf) = _txRepeatCount | _txAnswerEn;
	*(++buf) = '-'; //command - answer
	for (uint8_t i = 0; i < numBytes; ++i)	//message (values)
	{
		*(++buf) = str[i];
	}

	uint8_t length = buf - first;
	*first = _getCrc(first + 1, length);
	*(++buf) = utb_endByte;

	length = buf - utb_buffer + 1;
	for (int i = 0; i < _txRepeatCount; ++i)
	{
		__utb_uart_send(utb_buffer, length);
	}
}

#if FLASHEND < 1024
//only for tiny version
static void _sendAnswerTiny(unsigned char *str)
{
	unsigned char *buf = utb_buffer;
	*buf = utb_startByte;
	*(++buf) = _ownAddress;
	*(++buf) = *str;
	*(++buf) = utb_endByte;
	__utb_uart_send(utb_buffer, 4);
}

static utb_cmd_e _cmdChangeOutTiny(uint8_t val1, unsigned char * str)
{
	unsigned char val2 = *(str + 1);
	switch(val1)
	{
		case 'a':
		{
			if (val2) return set_outs;
			else return reset_outs;
		}
		case '1':
		{
			if (val2) return set_out1;
			else return reset_out1;
		}
		case '2':
		{
			if (val2) return set_out2;
			else return reset_out2;
		}
	}
	return 0;
}
#endif

static utb_cmd_e _cmdChangeOut(uint8_t val1, unsigned char * str)
{
	uint8_t num = val1;
	uint8_t v = *(str + 1);
	if (v > 100)
	return 0;

	utb_portValue->num = num;
	utb_portValue->value = v;

	if (num == 'a')
	{
		return change_outs;
	}
	else
	{
		return change_out;
	}

	return 0;
}

static utb_cmd_e _cmdSetClock(unsigned char *str)
{
	uint8_t dw = *(str + 1);
	uint8_t hh = *(str + 2);
	uint8_t mm = *(str + 3);
	if (dw == 0 || dw > 6 || hh > 23 || mm > 59)
	return 0;

	utb_time->hour = hh;
	utb_time->min = mm;
	utb_time->sec = 0;
	utb_time->dayWeek = dw;

	return set_clock;
}

static utb_cmd_e _cmdSendSensors()
{
	uint8_t sz = utb_sensorsSize * 2;
	uint8_t str[sz];
	int8_t t = -1;
	for (uint8_t i = 0; i < utb_sensorsSize; ++i)
	{
		str[++t] = (*utb_sensorsValue[i]) >> 8;
		str[++t] = (*utb_sensorsValue[i]) & 0xFF;
	}
	_sendAnswer(str, sz);
	return get_sensors;
}

static utb_cmd_e _getCmd(unsigned char *str)
{
	unsigned char val1 = *(str + 1);
	switch(*str)
	{
		case 'a':
		{
			_ownAddress = val1;
			eeprom_write_byte(&_e_ownAddress, _ownAddress);
			return set_address;
		}
		
		case 'o':
		{
			//v - numPort, v2 - valuePort
			#if FLASHEND < 1024
			return _cmdChangeOutTiny(val1, str);
			#else
			return _cmdChangeOut(val1, str);
			#endif
		}

		case 'b': return get_address;

		#if FLASHEND > 1024 || UTB_REPEATER_ONLY
		case 't':
		{
			if (val1 == '0' || val1 == '1')
			{
				utb_iRepeater = val1 == '1';
				eeprom_write_byte(&_e_imRepeater, utb_iRepeater);
				return toggle_Repeater;
			}
		}

		#endif
		#if FLASHEND > 1024
		case 'c': return _cmdSetClock(str);
		case 'g': return _cmdSendSensors();
		#endif
	}

	return 0;
}

static bool _existRepeaterAddr(uint8_t i)
{
	--i;
	while(i > 0)
	{
		if (utb_buffer[i] == _ownAddress)
		{
			return true;
		}
		--i;
	}
	return false;
}

utb_cmd_e utb_getCmd()
{
	if (!_checkNewLine())
	{
		return 0;
	}
	//find address
	int8_t i = str_indexOf(utb_buffer, '^');
	if (i < 1)
	{
		return 0;
	}

	unsigned char addr = utb_buffer[i + 1];
	bool forMyAddr = addr == utb_commonAddr || addr == _ownAddress;
	
	if (!forMyAddr && !utb_iRepeater) //if not for all than check address
	{
		return 0;
	}

	if (_getCrc(utb_buffer + i - 1, utb_buf_counter - i + 1) != utb_buffer[i - 2])
	{
		return 0;
	}
	
	uint8_t repeats =  utb_buffer[i + 2];
	_txAnswerEn = repeats & 128; //last byte
	_txRepeatCount = repeats - _txAnswerEn;

	//get command
	utb_cmd_e c = 0;
	if (forMyAddr)
	{
		c = _getCmd(utb_buffer + i + 3);
		if (_txAnswerEn > 0)
		{
			#if FLASHEND > 1024
			if (c) _sendAnswer((unsigned char *)"\rOk\r", 4);
			else _sendAnswer((unsigned char *)"\rEr\r", 4);
			#else
			if (c) _sendAnswerTiny((unsigned char *)"Ok");
			else _sendAnswerTiny((unsigned char *)"Er");
			#endif
		}
	}

	#if FLASHEND > 1024
	if (utb_iRepeater && !_existRepeaterAddr(i))
	{
		_sendRepeat(utb_buffer);
	}
	#endif

	return c;
}
