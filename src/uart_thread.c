#include "maindefs.h"
#include <stdio.h>
#include "uart_thread.h"
#include "messages.h"
#include "debug.h"
#include "my_uart.h"
#include <plib/usart.h>
<<<<<<< HEAD
#include "my_i2c.h"
=======
>>>>>>> 6c56c1ca634cf0c77d330afc29a918694412ee90


// This is a "logical" thread that processes messages from the UART
// It is not a "real" thread because there is only the single main thread
// of execution on the PIC because we are not using an RTOS.

int uart_lthread(uart_thread_struct *uptr, int msgtype, int length, unsigned char *msgbuffer) {

    DEBUG_ON(UART_TX);
    DEBUG_OFF(UART_TX);

    if (msgtype == MSGT_OVERRUN) {
        // TODO handle when buffer overruns.
<<<<<<< HEAD
    } else if (msgtype == MSGT_UART_SEND) {
=======
    } else if (msgtype == MSGT_UART_DATA) {
>>>>>>> 6c56c1ca634cf0c77d330afc29a918694412ee90
        // Send Motor Command
        if (msgbuffer[0] == MOTOR_COMMAND) {
            length = 5;
            // Send ARM Commands to Master PIC.
            uart_retrieve_buffer(length, msgbuffer);
            // Set UART TXF interrupt flag
            PIE1bits.TX1IE = 0x1;
        }
<<<<<<< HEAD
    } else if(msgtype == MSGT_UART_RCV) {
        i2c_retrieve_buffer(length, msgbuffer);
=======
>>>>>>> 6c56c1ca634cf0c77d330afc29a918694412ee90
    }
}