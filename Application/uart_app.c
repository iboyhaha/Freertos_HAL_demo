#include "uart_app.h"
#include "UART_Driver.h"
#include <string.h>
// void uart_task_function(void *pvParameters)
// {  
//     char *msg = "Hello, World!\r\n";
//     uart_send_by_interrupt(msg,strlen(msg));
//     char buffer[100];
//     while(true){
//         /*测试uart收发数据*/
//         memset(buffer, 0, sizeof(buffer));
//         uart_recv_by_block(buffer, sizeof(buffer)-1);
//         uart_send_by_block(buffer, strlen(buffer));
//     }
// }

#include "uart_app.h"
#include "uart_driver.h"
#include <string.h>
#include <stdbool.h>

//uart测试任务回调函数：
void uart_task_function(void* arg)
{
    const char* str = "Welcome uart_test,Plase input CMD: \n";
    uart_send_by_interrupt((void*)str,strlen(str));
    char buf[128] = {0};
    while (true)
    {
        /* 测试uart串口收发数据 */
        memset(buf, 0,sizeof(buf));
        uart_recv_by_interrupt(buf,sizeof(buf)-1);
        //发给串口，在PC串口终端进行显示：
        uart_send_by_interrupt(buf,strlen(buf));
    }
}


