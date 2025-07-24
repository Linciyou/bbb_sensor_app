CC = gcc
CFLAGS = -Wall -Wextra -g -O2
LDFLAGS = -lm
SRCS = main.c mpu6050.c i2c_lcd.c
OBJS = $(SRCS:.c=.o)
TARGET = bbb_sensor_app

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Cleanup complete."