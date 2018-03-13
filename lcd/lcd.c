 /*
 * lcd.c
 *
 * Created: 19-Oct-17
 *  Author: yahor.halubchyk
 */

 #include <stdbool.h>
 #include <stdint.h>
 #include <util/delay.h>

 #include "lcd.h"

 #if LCD_TYPE == 1
 #include "lcd_hd44780.h"
 #define LCD_NAME hd44780
 #endif

 #define __lcd_glue_method(a,b,c) a##b##c
 #define _lcd_glue_method(b,c) __lcd_glue_method(lcd_,b,c)

 #define _lcd_init() _lcd_glue_method(LCD_NAME,_init())	// lcd_hd44780_init()
 #define _lcd_goto_xy(x,y) _lcd_glue_method(LCD_NAME,_goto_xy(x,y))	// lcd_hd44780_goto_xy(x,y)
 #define _lcd_putChar(b) _lcd_glue_method(LCD_NAME,_putChar(b)) // lcd_hd44780_putChar(b)
 #define _lcd_power(en) _lcd_glue_method(LCD_NAME,_power(en))
 #define _lcd_displayOn(en)	_lcd_glue_method(LCD_NAME, _displayOn(en))
 #define _lcd_cursorBlink(en) _lcd_glue_method(LCD_NAME,_cursorBlink(en))
 #define _lcd_cursorVisible(en) _lcd_glue_method(LCD_NAME,_cursorVisible(en))

 /*
 * Setup the LCD controller.  First, call the hardware initialization
 * function, then adjust the display attributes we want.
 */

 bool lcd_init()
 {
	 return _lcd_init(); //result => lcd_hd44780_init()
 }

 void lcd_goto_xy(uint8_t x, uint8_t y)
 {
	 lcd_x_position = x;
	 lcd_y_position = y;

	 _lcd_goto_xy(x,y);
 }

 void lcd_reset_xy()
 {
	 lcd_goto_xy(0,0);
 }

 void lcd_newLine()
 {
	 lcd_x_position = 0;
	 ++lcd_y_position;

	 if (lcd_y_position == LCD_SIZE_Y)
	 lcd_y_position	= 0;

	 lcd_goto_xy(lcd_x_position, lcd_y_position);
 }

 /*
 * Send character c to the LCD display.  After a '\n' has been seen,
 * the next character will first clear the display.
 */
 void lcd_putChar(unsigned char b)
 {
	 if (b == '\n')
	 {
		 lcd_newLine();
		 return;
	 }
	 if (lcd_x_position == LCD_SIZE_X)
	 {
		 lcd_newLine();
	 }
	 
	 ++lcd_x_position;

	 _lcd_putChar(b);
 }

 void lcd_putString(unsigned char *str)
 {
	 while (*str)
	 {
		 lcd_putChar(*str++);
	 }
 }

 void lcd_putStringf(const char *str)
 {
	 while (*str)
	 {
		 lcd_putChar(*str++);
	 }
 }

 void lcd_power(bool enable)
 {
	 _lcd_power(enable);
 }

 void lcd_displayOn(bool enable)
 {
	 _lcd_displayOn(enable);
 }

 void lcd_cursorVisible(bool enable)
 {
	 _lcd_cursorVisible(enable);
 }
 void lcd_cursorBlink(bool enable)
 {
	 _lcd_cursorBlink(enable);
 }

