#ifndef __USART2_H
#define __USART2_H
#include "stdio.h"	
#include "sys.h" 

#define USART2_MAX_RECV_LEN		100					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART2_RX_EN 			1								//0,������;1,����.

extern u8  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART2_RX_STA;   				 //��������״̬
extern u8 en_senddata, u2_str_len, s2;
extern u8 time_data_esp[22];

void USART2_Init(u32 My_BaudRate);
	
#endif
