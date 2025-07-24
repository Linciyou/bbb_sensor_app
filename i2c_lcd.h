#ifndef I2C_LCD_H
#define I2C_LCD_H
#include <stdint.h>
#define LCD_ADDR 0x27
int lcd_init(int fd);
void lcd_clear(int fd);
void lcd_set_cursor(int fd, uint8_t col, uint8_t row);
void lcd_puts(int fd, const char *s);
void lcd_command(int fd, uint8_t cmd);
#endif // I2C_LCD_H