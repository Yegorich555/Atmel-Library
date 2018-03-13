 /*
 * lcd.h
 *
 * Created: 19-Oct-17
 *  Author: yahor.halubchyk
 */
 #include <stdint.h>
 #include <stdbool.h>

 #ifndef LCD_H_
 #define LCD_H_

 #ifndef LCD_AUTOINC_X
 #define LCD_AUTOINC_X 1
 #endif

 #ifndef LCD_AUTOINC_Y
 #define LCD_AUTOINC_Y 1
 #endif

 uint8_t lcd_x_position;
 uint8_t lcd_y_position;
 
 /*
 LCD_TYPES:
 1 = HD44780
 2 = Nokia3310
 */

 #ifndef LCD_TYPE
 #define LCD_TYPE 1
 #endif

 #if LCD_TYPE == 1

  //***************   LCD == 1        HD44780
 //#pragma message "LCD_TYPE = hd44780"
 #ifndef LCD_SIZE_XY
 #pragma message "set default => LCD_SIZE_XY = 16, 2"
 #define LCD_SIZE_XY 16, 2
 #endif

 #ifndef LCD_USE_BUSY_BIT
 #define LCD_USE_BUSY_BIT 1
 #endif

 #ifndef LCD_IO_RS
 #error "LCD_IO_RS didn't define"
 #define LCD_IO_RS A, 0
 #endif

 #ifndef LCD_IO_RW
 #error "LCD_IO_RW didn't define"
 #define LCD_IO_RW A, 1
 #endif

 #ifndef LCD_IO_E
 #error "LCD_IO_E didn't define"
 #define LCD_IO_E A, 2
 #endif

 #ifndef LCD_IO_D4
 #error "LCD_IO_D4 didn't define"
 #define LCD_IO_D4 B, 0
 #endif

 #ifndef LCD_IO_D5
 #error "LCD_IO_D5 didn't define"
 #define LCD_IO_D5 B, 1
 #endif

 #ifndef LCD_IO_D6
 #error "LCD_IO_D6 didn't define"
 #define LCD_IO_D6 B, 2
 #endif

 #ifndef LCD_IO_D7
 #error "LCD_IO_D7 didn't define"
 #define LCD_IO_D7 B, 3
 #endif

 #endif	//endif LCD_TYPE

 #define __lcd_size_x(x, y) x
 #define _lcd_size_x(size) __lcd_size_x(size)
 #define LCD_SIZE_X _lcd_size_x(LCD_SIZE_XY)

 #define __lcd_size_y(x, y) y
 #define _lcd_size_y(size) __lcd_size_y(size)
 #define LCD_SIZE_Y _lcd_size_y(LCD_SIZE_XY)

 bool lcd_init();

 void lcd_clear();
 void lcd_goto_xy(uint8_t x, uint8_t y);
 void lcd_reset_xy();
 void lcd_newLine();

 void lcd_putChar(unsigned char c);
 void lcd_putString(unsigned char *str);
 void lcd_putStringf(const char *str);

 void lcd_power(bool enable);
 void lcd_displayOn(bool enable);
 void lcd_cursorVisible(bool enable);
 void lcd_cursorBlink(bool enable);

 #include "lcd.c"
 #endif /* LCD_H_ */
