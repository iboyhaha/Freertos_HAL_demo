#include <stdio.h>
#include<stdbool.h>
#include <stdint.h>
#include "UART_Driver.h"
#include "FreeRTOS.h"
#include "usart.h"
#include <stm32f4xx_hal_uart.h>
#include "main.h"
#include "task.h"


extern UART_HandleTypeDef huart1;
//阻塞方式发送数据：
int uart_send_by_block(void* buf, int size)
{
    return HAL_UART_Transmit(&huart1,buf, size, portMAX_DELAY);
}
//阻塞方式接收数据：
int uart_recv_by_block(void* buf, int maxSize)
{
    HAL_StatusTypeDef status;
    char* ptr = (char*)buf;
    int len = 0;
    while(true)
    {
        status = HAL_UART_Receive(&huart1, (uint8_t*)ptr, 1, portMAX_DELAY);
        if(*ptr == '\0' || *ptr == '\n' || *ptr == '\r')
        {
            break;
        }
        ptr++;
        len++;
        if(len > maxSize - 1)//应该是len > maxSize -1 才对，有点疏忽了。
        {
            break;
        }
    }
    return status;    
}

SemaphoreHandle_t send_sem;
int uart_send_by_interrupt(void* buf, int size){
    HAL_StatusTypeDef status;
    status = HAL_UART_Transmit_IT(&huart1, buf, size);
    send_sem = xSemaphoreCreateBinary();
    xSemaphoreTake(send_sem, portMAX_DELAY);
    //删除信号量
    vSemaphoreDelete(send_sem);
    return status;
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    BaseType_t status;
    //释放信号量
    xSemaphoreGiveFromISR(send_sem,&status);
    if(status == pdTRUE){
        portYIELD_FROM_ISR(status);
    }
}

//非阻塞方式接收数据：
SemaphoreHandle_t recv_sem;
char* ptr = NULL;
int maxlen = 0;
int uart_recv_by_interrupt(void* buf, int maxSize)
{
    HAL_StatusTypeDef status;
    ptr = buf;
    maxlen = maxSize;
    //先申请信号量
    recv_sem = xSemaphoreCreateBinary();
    //再调用非阻塞中断接收函数：
    status = HAL_UART_Receive_IT(&huart1, buf, 1);
    
    //获取信号量：
    xSemaphoreTake(recv_sem, portMAX_DELAY);
    //删除信号量：
    vSemaphoreDelete(recv_sem);
    return status;
}

//接受中断中的完成时的中断回调函数：
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static int len = 0;
    BaseType_t status;
    if(*ptr == '\0' || *ptr == '\n' || *ptr == '\r' || len > maxlen - 1)
    {
        //归还信号量：
        xSemaphoreGiveFromISR(recv_sem, &status);
        if(status == pdTRUE)
        {
            portYIELD_FROM_ISR(status);
        }
        //len归0
        len  = 0;
        //一定要在此返回
        return;
    }
    len++;
    ptr++;
    HAL_UART_Receive_IT(&huart1, (uint8_t*)ptr, 1);
}

