#include "mpu6050.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_XOUT_H  0x43
static int16_t read_word_2c(int fd, uint8_t reg_addr) {
    uint8_t buffer[2];
    if (write(fd, &reg_addr, 1) != 1) { perror("mpu6050: Failed to set register pointer"); return 0; }
    if (read(fd, buffer, 2) != 2) { perror("mpu6050: Failed to read word"); return 0; }
    return (int16_t)((buffer[0] << 8) | buffer[1]);
}
int mpu6050_init(int fd) {
    if (ioctl(fd, I2C_SLAVE, MPU6050_ADDR) < 0) { perror("mpu6050: Failed to set slave address"); return -1; }
    uint8_t buffer[2] = {MPU6050_PWR_MGMT_1, 0x00};
    if (write(fd, buffer, 2) != 2) { perror("mpu6050: Failed to wake up sensor"); return -1; }
    return 0;
}
int mpu6050_read_all(int fd, MPU6050_Data_t *data) {
    if (ioctl(fd, I2C_SLAVE, MPU6050_ADDR) < 0) { perror("mpu6050: Failed to set slave address for reading"); return -1; }
    int16_t ax_raw = read_word_2c(fd, MPU6050_ACCEL_XOUT_H);
    int16_t ay_raw = read_word_2c(fd, MPU6050_ACCEL_XOUT_H + 2);
    int16_t az_raw = read_word_2c(fd, MPU6050_ACCEL_XOUT_H + 4);
    int16_t gx_raw = read_word_2c(fd, MPU6050_GYRO_XOUT_H);
    int16_t gy_raw = read_word_2c(fd, MPU6050_GYRO_XOUT_H + 2);
    int16_t gz_raw = read_word_2c(fd, MPU6050_GYRO_XOUT_H + 4);
    data->accel.x = (float)ax_raw / 16384.0f; data->accel.y = (float)ay_raw / 16384.0f; data->accel.z = (float)az_raw / 16384.0f;
    data->gyro.x = (float)gx_raw / 131.0f; data->gyro.y = (float)gy_raw / 131.0f; data->gyro.z = (float)gz_raw / 131.0f;
    return 0;
}