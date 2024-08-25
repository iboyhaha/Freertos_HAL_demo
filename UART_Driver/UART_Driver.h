#ifndef __UART_DRIVE_H__
#define __UART_DRIVE_H__
#include<stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"



int uart_send_by_block(void* buf, int size);
int uart_recv_by_block(void* buf, int maxSize);

//非阻塞
int uart_send_by_interrupt(void* buf, int size);
int uart_recv_by_interrupt(void* buf, int maxSize);

#endif // !__UART_DRIVE_H__
