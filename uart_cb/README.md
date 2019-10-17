# ESP8266 串口接收 驱动

ESP8266 NONOS SDK 串口接收 驱动
* 回调函数定义：`typedef void (*recv_uart_callback)(uint8* data, uint16 data_len)`
* 设置接收回调函数：`set_uart_cb(recv_uart_callback)`

