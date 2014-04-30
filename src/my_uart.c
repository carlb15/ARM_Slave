#include "maindefs.h"
#ifndef __XC8
#include <usart.h>
#else
#include <plib/usart.h>
#endif
#include "my_uart.h"
#include "debug.h"
#include "my_i2c.h"

static uart_comm *uc_ptr;
unsigned char msgtype_uart = MSGTYPE;

void uart_recv_int_handler() {

    // Write a byte (one character) to the usart transmit buffer.
    // If 9-bit mode is enabled, the 9th bit is written from the field TX_NINE,
    // found in a union of type USART

#ifdef __USE18F26J50
    if (DataRdy1USART()) {
        uc_ptr->Rx_buffer[uc_ptr->Rx_buflen] = Read1USART();
#else
#ifdef __USE18F46J50
    if (DataRdy1USART()) {
        char data = Read1USART();
#else
    if (DataRdyUSART()) {
        char data = ReadUSART();
#endif
#endif

        switch (msgtype_uart) {
            case MSGTYPE:
                uc_ptr->Rx_buffer[0] = data;
                uc_ptr->Rx_buflen++;
                msgtype_uart = LENGTH;
                break;

            case LENGTH:
                if (uc_ptr->Rx_buflen == 1) {
                    uc_ptr->Rx_buffer[uc_ptr->Rx_buflen] = data;
                    uc_ptr->Rx_buflen++;
                    msgtype_uart = MESSAGE;
                } else {
                    uc_ptr->Rx_buflen = 0;
                    msgtype_uart = MSGTYPE;
                }
                break;

            case MESSAGE:
                if (uc_ptr->Rx_buflen > uc_ptr->Rx_buffer[1]) {
                    uc_ptr->Rx_buffer[uc_ptr->Rx_buflen] = data;
                    uc_ptr->Rx_buflen++;
                    msgtype_uart = CHECKSUM;
                } else {
                    uc_ptr->Rx_buffer[uc_ptr->Rx_buflen] = data;
                    uc_ptr->Rx_buflen++;
                    msgtype_uart = MESSAGE;
                }
                break;

            case CHECKSUM:
                if (uc_ptr->Rx_buflen > uc_ptr->Rx_buffer[1]) {
                    uc_ptr->Rx_buffer[uc_ptr->Rx_buflen] = data;
                    uc_ptr->Rx_buflen++;
                    unsigned char checkSum = 0;
                    unsigned char bufLength = uc_ptr->Rx_buffer[1];

                    // Check for correct checksum.
                    int i = 0;
                    for (; i < uc_ptr->Rx_buffer[1]; i++) {
                        checkSum ^= uc_ptr->Rx_buffer[i + 2];
                    }

                    if (uc_ptr->Rx_buflen != uc_ptr->Rx_buffer[1] + 3) {
                        // Message has been scrambled.
                        uc_ptr->Rx_buffer[0] == COMMAND_NACK;
                    }

                    if (checkSum != uc_ptr->Rx_buffer[uc_ptr->Rx_buflen - 1] || uc_ptr->Rx_buffer[0] == COMMAND_NACK) {
                        // Send previous message again.
                        ToMainHigh_sendmsg(uc_ptr->Tx_buflen, MSGT_UART_SEND, (void *) uc_ptr->Tx_buffer);
                    } else if (uc_ptr->Rx_buffer[0] != COMMAND_ACK) {
                        // Send sensor or motor encoder data to the ARM.
                        ToMainHigh_sendmsg(uc_ptr->Rx_buflen, MSGT_UART_RCV, (void *) uc_ptr->Rx_buffer);
                        // Return an ACK to the Master.
                        ToMainLow_sendmsg(CMD_ACKLEN, MSGT_UART_SEND, (void *) uc_ptr->cmd_ack_buf);
                    } else if (uc_ptr->Rx_buffer[0] == COMMAND_ACK) {
                        // Allow ARM to send a new motor command.
                        motorCommandSent = 1;
                    }
                }
                msgtype_uart = MSGTYPE;
                uc_ptr->Rx_buflen = 0;
                break;

            default:
                break;
        }
    }

#ifdef __USE18F26J50
    if (USART1_Status.OVERRUN_ERROR == 1) {
#else
#ifdef __USE18F46J50
    if (USART1_Status.OVERRUN_ERROR == 1) {
#else
    if (USART_Status.OVERRUN_ERROR == 1) {
#endif
#endif
        // we've overrun the USART and must reset
        // send an error message for this
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
        ToMainLow_sendmsg(0, MSGT_OVERRUN, (void *) 0);
    }
}

void uart_send_int_handler() {

    if (uc_ptr->Tx_buflen == uc_ptr->msg_length) {
        PIE1bits.TX1IE = 0; // Clear TXIE to end write.
        uc_ptr->Tx_buflen = 0;
    } else {
        Write1USART(uc_ptr->Tx_buffer[uc_ptr->Tx_buflen++]);
    }
}

void init_uart_recv(uart_comm * uc) {
    uc_ptr = uc;
    uc_ptr->Tx_buflen = 0;
    uc_ptr->Rx_buflen = 0;
    uc_ptr->msg_length = 0;

    // Intialize CMD ACK response.
    uc_ptr->cmd_ack_buf[0] = 0x10;
    uc_ptr->cmd_ack_buf[1] = 0x01;
    uc_ptr->cmd_ack_buf[2] = 0x02;
    uc_ptr->cmd_ack_buf[3] = 0x02;
}

void uart_retrieve_buffer(int length, unsigned char* msgbuffer) {

    int i = 0;
    uc_ptr->Tx_buflen = 0;
    uc_ptr->msg_length = length;

    for (; i < length; i++) {
        uc_ptr->Tx_buffer[i] = msgbuffer[i];
    }
    // Set UART TXF interrupt flag
    PIE1bits.TX1IE = 0x1;
}