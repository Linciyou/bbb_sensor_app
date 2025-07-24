#include "i2c_lcd.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#define LCD_CLEARDISPLAY 0x01
#define LCD_SETDDRAMADDR 0x80
#define LCD_FUNCTIONSET 0x20
#define LCD_DISPLAYCONTROL 0x08
#define LCD_ENTRYMODESET 0x04
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_5x8DOTS 0x00
#define LCD_DISPLAYON 0x04
#define LCD_CURSOROFF 0x00
#define LCD_BLINKOFF 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTDECREMENT 0x00
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE_BIT 0x04
static void lcd_send(int fd, uint8_t value, uint8_t rs_mode);
static void lcd_write_nibble(int fd, uint8_t nibble, uint8_t rs_mode);
static void lcd_write_nibble(int fd, uint8_t nibble, uint8_t rs_mode) {
    uint8_t data = (nibble << 4) | rs_mode | LCD_BACKLIGHT;
    uint8_t pulse1 = data | LCD_ENABLE_BIT;
    uint8_t pulse2 = data & ~LCD_ENABLE_BIT;
    write(fd, &pulse1, 1); usleep(500);
    write(fd, &pulse2, 1); usleep(100);
}
static void lcd_send(int fd, uint8_t value, uint8_t rs_mode) {
    uint8_t high_nibble = value & 0xF0;
    uint8_t low_nibble = (value << 4) & 0xF0;
    lcd_write_nibble(fd, high_nibble >> 4, rs_mode);
    lcd_write_nibble(fd, low_nibble >> 4, rs_mode);
}
void lcd_command(int fd, uint8_t cmd) { lcd_send(fd, cmd, 0); }
void lcd_clear(int fd) { lcd_command(fd, LCD_CLEARDISPLAY); usleep(2000); }
void lcd_set_cursor(int fd, uint8_t col, uint8_t row) {
    const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > 1) { row = 1; }
    lcd_command(fd, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}
void lcd_puts(int fd, const char *s) { while (*s) { lcd_send(fd, *s++, 1); } }
int lcd_init(int fd) {
    if (ioctl(fd, I2C_SLAVE, LCD_ADDR) < 0) { perror("lcd: Failed to set slave address"); return -1; }
    usleep(50000);
    lcd_write_nibble(fd, 0x03, 0); usleep(4500);
    lcd_write_nibble(fd, 0x03, 0); usleep(4500);
    lcd_write_nibble(fd, 0x03, 0); usleep(150);
    lcd_write_nibble(fd, 0x02, 0);
    lcd_command(fd, LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    lcd_command(fd, LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    lcd_command(fd, LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
    lcd_clear(fd);
    return 0;
}