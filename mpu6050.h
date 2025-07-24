#ifndef MPU6050_H
#define MPU6050_H
#include <stdint.h>
#define MPU6050_ADDR 0x68
typedef struct { float x, y, z; } Vector3D_t;
typedef struct { Vector3D_t accel; Vector3D_t gyro; } MPU6050_Data_t;
int mpu6050_init(int fd);
int mpu6050_read_all(int fd, MPU6050_Data_t *data);
#endif // MPU6050_H