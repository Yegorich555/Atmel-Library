/*
* ----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
* ----------------------------------------------------------------------------
*
* HD44780 LCD display driver
*
* The LCD controller is used in 4-bit mode with a full bi-directional
* interface (i.e. R/~W is connected) so the busy flag can be read.
*
* $Id$
*/

#include "extensions.h"
#include "lcd_hd44780.h"

#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

static bool _dispEn;
static bool _dispCursorBlink;
static bool _dipsCursorVis;

/* Send a command to the LCD controller. */
#define lcd_hd44780_outcmd(n)	lcd_hd44780_outbyte((n), 0)

/* Send a data byte to the LCD controller. */
#define lcd_hd44780_outdata(n)	lcd_hd44780_outbyte((n), 1)

/* Read the address counter and busy flag from the LCD. */
#define lcd_hd44780_incmd() lcd_hd44780_inbyte(0)

/* Read the current data byte from the LCD. */
#define lcd_hd44780_indata() lcd_hd44780_inbyte(1)

/* Clear LCD display command. */
#define LCD_HD44780_CLR 0x01

/* Home cursor command. */
#define LCD_HD44780_HOME 0x02

/* Display mode command */
#define LCD_HD44780_CMD_Disp 0x08

#define LCD_HD44780_BUSYFLAG 0x80

/*
* Select the entry mode.  inc determines whether the address counter
* auto-increments, shift selects an automatic display shift.
*/
#define lcd_hd44780_entmode(inc, shift) \
(0x04 | ((inc)? 0x02: 0) | ((shift)? 1: 0))

/*
* Selects disp[lay] on/off, cursor on/off, cursor blink[ing]
* on/off.
*/
#define lcd_hd44780_dispSetMode(disp, cursor, blink) \
(0x08 | ((disp)? 0x04: 0) | ((cursor)? 0x02: 0) | ((blink)? 1: 0))

/*
* With shift = 1, shift display right or left.
* With shift = 0, move cursor right or left.
*/
#define lcd_hd44780_shift(shift, right) \
(0x10 | ((shift)? 0x08: 0) | ((right)? 0x04: 0))

/*
* Function set.  if8bit selects an 8-bit data path, twoline arranges
* for a two-line display, font5x10 selects the 5x10 dot font (5x8
* dots if clear).
*/
#define lcd_hd44780_fnset(if8bit, twoline, font5x10) \
(0x20 | ((if8bit)? 0x10: 0) | ((twoline)? 0x08: 0) | \
((font5x10)? 0x04: 0))

/*
* Set the next character generator address to addr.
*/
#define lcd_hd44780_cgaddr(addr) \
(0x40 | ((addr) & 0x3f))

/*
* Set the next display address to addr.
*/
#define lcd_hd44780_ddaddr(addr) \
(0x80 | ((addr) & 0x7f))

/*
* Send one pulse to the E signal (enable).  Mind the timing
* constraints.  If readback is set to true, read the HD44780 data
* pins right before the falling edge of E, and return that value.
*/
static inline uint8_t lcd_hd44780_pulse_e(bool readback) __attribute__((always_inline));

static inline uint8_t lcd_hd44780_pulse_e(bool readback)
{
	uint8_t x;
	io_setPort(LCD_IO_E);
	_delay_us(1);
	if (readback)
	{
		x = (io_getPin(LCD_IO_D4) >> io_bitNumber(LCD_IO_D4)) |
		(io_getPin(LCD_IO_D5) >> (io_bitNumber(LCD_IO_D5) - 1)) |
		(io_getPin(LCD_IO_D6) >> (io_bitNumber(LCD_IO_D6) - 2)) |
		(io_getPin(LCD_IO_D7) >> (io_bitNumber(LCD_IO_D7) - 3));
	}
	else
	{
		x = 0;
	}
	io_resetPort(LCD_IO_E);

	return x;
}

/*
* Send one nibble out to the LCD controller.
*/
static void	lcd_hd44780_outnibble(uint8_t n, uint8_t rs)
{
	io_resetPort(LCD_IO_RW);
	if (rs)	io_setPort(LCD_IO_RS);
	else io_resetPort(LCD_IO_RS);
	//ASSIGN(PORT, LCD_IO_D4, n);

	io_change(PORT, LCD_IO_D4, n & 1);
	io_change(PORT, LCD_IO_D5, n & (1<<1));
	io_change(PORT, LCD_IO_D6, n & (1<<2));
	io_change(PORT, LCD_IO_D7, n & (1<<3));
	(void)lcd_hd44780_pulse_e(false);
}

/*
* Send one byte to the LCD controller.  As we are in 4-bit mode, we
* have to send two nibbles.
*/
static void lcd_hd44780_outbyte(uint8_t b, uint8_t rs)
{
	lcd_hd44780_outnibble(b >> 4, rs);
	lcd_hd44780_outnibble(b & 0xf, rs);
}

/*
* Read one nibble from the LCD controller.
*/
static void lcd_setPortsOutput()
{
	io_set(DDR, LCD_IO_D4);
	io_set(DDR, LCD_IO_D5);
	io_set(DDR, LCD_IO_D6);
	io_set(DDR, LCD_IO_D7);
}

static void lcd_setPortsInput()
{
	io_reset(DDR, LCD_IO_D4);
	io_reset(DDR, LCD_IO_D5);
	io_reset(DDR, LCD_IO_D6);
	io_reset(DDR, LCD_IO_D7);
}

static uint8_t lcd_hd44780_innibble(uint8_t rs)
{
	uint8_t x;

	io_setPort(LCD_IO_RW);
	lcd_setPortsInput();
	if (rs)	io_setPort(LCD_IO_RS);
	else io_resetPort(LCD_IO_RS);
	x = lcd_hd44780_pulse_e(true);
	lcd_setPortsOutput();
	io_resetPort(LCD_IO_RW);
	return x;
}

/*
* Read one byte (i.e. two nibbles) from the LCD controller.
*/
static uint8_t	lcd_hd44780_inbyte(uint8_t rs)
{
	uint8_t x;

	x = lcd_hd44780_innibble(rs) << 4;
	x |= lcd_hd44780_innibble(rs);

	return x;
}

/*
* Wait until the busy flag is cleared.
*/
static bool lcd_hd44780_wait_ready(bool longwait)
{
	#if LCD_USE_BUSY_BIT
	uint8_t cnt = 0;
	while (lcd_hd44780_incmd() & LCD_HD44780_BUSYFLAG)
	{
		if (++cnt > 200) return false;
		_delay_us(10);
	}
	#else
	if (longwait)
	_delay_ms(2); // 1.53 ms
	else
	_delay_us(200); // > 37us
	#endif

	return true;
}

void lcd_hd44780_setDisplayMode(bool dispEnable, bool cursorVisible, bool blinkEnable)
{
	lcd_hd44780_outcmd(lcd_hd44780_dispSetMode(dispEnable, cursorVisible, blinkEnable));
	lcd_hd44780_wait_ready(false);
}

void lcd_hd44780_displayOn(bool enable)
{
	_dispEn = enable;
	lcd_hd44780_setDisplayMode(_dispEn, _dipsCursorVis, _dispCursorBlink);
}

void lcd_hd44780_cursorVisible(bool enable)
{
	_dipsCursorVis = enable;
	lcd_hd44780_setDisplayMode(_dispEn, _dipsCursorVis, _dispCursorBlink);
}

void lcd_hd44780_cursorBlink(bool enable)
{
	_dispCursorBlink = enable;
	lcd_hd44780_setDisplayMode(_dispEn, _dipsCursorVis, _dispCursorBlink);
}

void lcd_hd44780_clear()
{
	lcd_hd44780_outcmd(LCD_HD44780_CLR);
	lcd_hd44780_wait_ready(false);
}

bool lcd_hd44780_init(void)
{
	io_set(DDR, LCD_IO_RS);
	io_set(DDR, LCD_IO_RW);
	io_set(DDR, LCD_IO_E);
	lcd_setPortsOutput();

	#define _initCmd 0b0011
	_delay_ms(17); //15ms		/* 40 ms needed for Vcc = 2.7 V */
	lcd_hd44780_outnibble(_initCmd, 0);
	_delay_ms(5);  //4.1ms
	lcd_hd44780_outnibble(_initCmd, 0);
	_delay_us(150); //100us
	lcd_hd44780_outnibble(_initCmd, 0);
	_delay_us(50); //37us
	lcd_hd44780_outnibble(0b0010, 0);
	if (!lcd_hd44780_wait_ready(false))
	{
		return false;
	}

	#if LCD_SIZE_Y > 1
	#define _twoLine 1
	#else
	#define _twoLine 0
	#endif
	// Function set: if8bit, twoline, font5x10 (5x8 dots if clear).
	lcd_hd44780_outcmd(lcd_hd44780_fnset(0, _twoLine, 1));
	lcd_hd44780_wait_ready(false);

	// Entry mode: auto-increment address counter, no display shift in
	lcd_hd44780_outcmd(lcd_hd44780_entmode(1, 0));
	lcd_hd44780_wait_ready(false);

	lcd_hd44780_clear();

	//lcd_hd44780_outcmd(LCD_HD44780_HOME);
	//lcd_hd44780_wait_ready(true);

	lcd_hd44780_setDisplayMode(true, false, false);

	return true; // return success from init
}

//const uint8_t _base_y[4] = {0x80, 0xC0, 0x90, 0xD0};
const uint8_t _base_y[4] = {0x80, 0x80 + 0x40, 0x80 + 0x14, 0x80 + 0x54};
void lcd_hd44780_goto_xy(uint8_t x, uint8_t y)
{
	lcd_hd44780_outcmd(_base_y[y] + x);
	lcd_hd44780_wait_ready(false);
}

void lcd_hd44780_putChar(unsigned char b)
{
	lcd_hd44780_outdata(b);
	lcd_hd44780_wait_ready(false);
}

void lcd_hd44780_power(bool enable)
{
	if (enable)
	{
		lcd_hd44780_displayOn(enable);
	}
	else
	{
		lcd_hd44780_setDisplayMode(0,0,0);
		io_resetPort(LCD_IO_D4);
		io_resetPort(LCD_IO_D5);
		io_resetPort(LCD_IO_D6);
		io_resetPort(LCD_IO_D7);

		io_resetPort(LCD_IO_RS);
		io_resetPort(LCD_IO_RW);
		io_resetPort(LCD_IO_E);
	}
}

