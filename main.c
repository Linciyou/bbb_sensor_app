#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "mpu6050.h"
#include "i2c_lcd.h"
#define MPU_I2C_BUS  "/dev/i2c-1"
#define LCD_I2C_BUS  "/dev/i2c-2"
#define UPDATE_INTERVAL_US 200000 
#define DISPLAY_SWITCH_INTERVAL 10
int main() {
    int fd_mpu, fd_lcd;
    if ((fd_mpu = open(MPU_I2C_BUS, O_RDWR)) < 0) { perror("Failed to open MPU6050 bus"); return 1; }
    if ((fd_lcd = open(LCD_I2C_BUS, O_RDWR)) < 0) { perror("Failed to open LCD bus"); close(fd_mpu); return 1; }
    if (mpu6050_init(fd_mpu) != 0) { fprintf(stderr, "Failed to initialize MPU6050\n"); return 1; }
    if (lcd_init(fd_lcd) != 0) { fprintf(stderr, "Failed to initialize LCD\n"); return 1; }
    printf("Initialization complete. Starting main loop...\n");
    MPU6050_Data_t sensor_data;
    int loop_counter = 0;
    int display_mode = 0;
    while (1) {
        if (mpu6050_read_all(fd_mpu, &sensor_data) != 0) {
            fprintf(stderr, "Warning: Failed to read sensor data.\n");
            usleep(UPDATE_INTERVAL_US);
            continue;
        }

        if (loop_counter++ >= DISPLAY_SWITCH_INTERVAL) {
            loop_counter = 0;
            display_mode = !display_mode;
        }

        char line1[17], line2[17];
        if (display_mode == 0) { // Accel Mode
            snprintf(line1, sizeof(line1), "ACCEL (g)    *");
            snprintf(line2, sizeof(line2), "X%+.1fY%+.1fZ%+.1f ", sensor_data.accel.x, sensor_data.accel.y, sensor_data.accel.z);
        
        } else { // Gyro Mode
            snprintf(line1, sizeof(line1), "GYRO (%c/s)   *", 223);
            snprintf(line2, sizeof(line2), "X%-4.0fY%-4.0fZ%-4.0f ", sensor_data.gyro.x, sensor_data.gyro.y, sensor_data.gyro.z);
        }

        lcd_set_cursor(fd_lcd, 0, 0);
        lcd_puts(fd_lcd, line1);
        lcd_set_cursor(fd_lcd, 0, 1);
        lcd_puts(fd_lcd, line2);

        usleep(UPDATE_INTERVAL_US);
    }
    close(fd_mpu); close(fd_lcd);
    return 0;
}