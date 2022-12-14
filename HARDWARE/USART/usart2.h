#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 

#define USART2_MAX_RECV_LEN		100					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART2_RX_EN 			1								//0,不接收;1,接收.

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART2_RX_STA;   				 //接收数据状态
extern u8 en_senddata, u2_str_len, s2;
extern u8 time_data_esp[22];

void USART2_Init(u32 My_BaudRate);
	
#endif

