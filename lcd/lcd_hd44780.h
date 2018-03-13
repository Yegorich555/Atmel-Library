 /*
 * lcd_hd44780.h
 * driver fo lcd display
 * Created: 19-Oct-17
 *  Author: yahor.halubchyk
 */

 #ifndef LCD_HD44780_H_
 #define LCD_HD44780_H_

 bool lcd_hd44780_init();
 void lcd_hd44780_clear();
 void lcd_hd44780_goto_xy(uint8_t x, uint8_t y);
 void lcd_hd44780_putChar(unsigned char b);

 void lcd_hd44780_power(bool enable);
 void lcd_hd44780_displayOn(bool enable);
 void lcd_hd44780_cursorVisible(bool enable);
 void lcd_hd44780_cursorBlink(bool enable);

 #include "lcd_hd44780.c"
 #endif /* LCD_HD44780_H_ */