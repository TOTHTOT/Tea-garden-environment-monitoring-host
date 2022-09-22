#include "exti.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "lora.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//�ⲿ�ж� ��������			   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2010/12/01  
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  

u8 Slave_Page = 1;
//�ⲿ�жϳ�ʼ������
void EXTIX_Init(void)
{
 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PC�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PC�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//�ⲿ�жϣ���Ҫʹ��AFIOʱ��
	
	//�����ж�
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��PC13
    GPIO_SetBits(GPIOA,GPIO_Pin_7);						    //PC13���				
	 
	
/* 	//��ťS1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 // �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 			 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
	//��ťs2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 // �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_13);
	
	
	//��ťs3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 // �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	
	//��ťs3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 // �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_15); */
	
    //GPIOA.7 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

/* 	 //GPIOB.12 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	 //GPIOB.13 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line13;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	 //GPIOB.14 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line14;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
	
	//GPIOB.15 �ж����Լ��жϳ�ʼ������
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ��� */
	
 	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
		
/* 	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//��ռ���ȼ�0�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//�����ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  */
 
}

 

 void EXTI9_5_IRQHandler(void)
{			
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 0)
		{
			Send_Data(0x00, 0x01, 0x17, "&&reboot$$");
			Send_Data(0x00, 0x02, 0x17, "&&reboot$$");
			Send_Data(0x00, 0x03, 0x17, "&&reboot$$");
			Send_Data(0x00, 0x04, 0x17, "&&reboot$$");
			
			printf("����\r\n");
			delay_ms(10);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}
		
	
 	 EXTI_ClearITPendingBit(EXTI_Line7);    //���LINE5�ϵ��жϱ�־λ  
}

/* void EXTI15_10_IRQHandler(void)
{
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
	{
		Slave_Page = 1;
	}
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
	{
		Slave_Page = 2;
	}
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14))
	{
		Slave_Page = 3;
	}
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15))
	{
		Slave_Page = 4;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line12);		//����жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line13);		//����жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line14);		//����жϱ�־λ
	EXTI_ClearITPendingBit(EXTI_Line15);		//����жϱ�־λ
}
 */
