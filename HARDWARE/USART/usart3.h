#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  
 
#define USART3_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_RX_EN 			1					//0,不接收;1,接收.
 
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART3_RX_STA;   						//接收数据状态
extern u8 check_cmd_result, cmd_flag,str_len, s1;

void usart3_init(u32 bound);				//串口3初始化 
void u3_printf(char* fmt,...);
void USART3_RX_Data(void);
u8 Check_LORA_Return_Is_OK(void);
void u3_send_byte(unsigned char SendData);
unsigned char u3_get_byte(unsigned char* GetData);
#endif
 
 
 
 
 
 
 
 
 
 
 
 

