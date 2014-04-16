#ifndef __my_i2c_h
#define __my_i2c_h

#include "messages.h"

#define MAXI2CBUF MSGLEN
#define CMDACKLEN 4
#define SENSORLEN 6
#define MOTORLEN 5
#define MOTORCMDLEN 6

typedef struct __i2c_comm {
    unsigned char buffer[MAXI2CBUF];
    unsigned char buflen;
    unsigned char event_count;
    unsigned char status;
    unsigned char error_code;
    unsigned char error_count;
    unsigned char outbuffer[MAXI2CBUF];
    unsigned char outbuflen;
    unsigned char outbufind;
    unsigned char slave_addr;
    unsigned char cmd_ack_buf[CMDACKLEN];
    unsigned char sensor_buffer[MAXI2CBUF];
    unsigned char motor_buffer[MAXI2CBUF];
    unsigned char no_encoder_buffer[MAXI2CBUF];
    unsigned char no_sensor_buffer[MAXI2CBUF];
} i2c_comm;

int validSensorFlag, validMotorFlag;
unsigned char motorBuff[6];

#define I2C_IDLE 0x5
#define I2C_STARTED 0x6
#define	I2C_RCV_DATA 0x7
#define I2C_SLAVE_SEND 0x8

#define I2C_ERR_THRESHOLD 1
#define I2C_ERR_OVERRUN 0x4
#define I2C_ERR_NOADDR 0x5
#define I2C_ERR_NODATA 0x6
#define I2C_ERR_MSGTOOLONG 0x7
#define I2C_ERR_MSG_TRUNC 0x8

void init_i2c(i2c_comm *);
void i2c_int_handler(void);
void start_i2c_slave_reply(unsigned char, unsigned char *);
void i2c_configure_slave(unsigned char);
void readMessages();
void pass_motor_values_to_i2c(unsigned char* msgbuffer, unsigned char length);
void pass_sensor_values_to_i2c(unsigned char* msgbuffer, unsigned char length);
#endif