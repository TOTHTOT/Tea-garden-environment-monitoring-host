#include "exti.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "lora.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//Mini STM32开发板
//外部中断 驱动代码			   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2010/12/01  
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved	  
////////////////////////////////////////////////////////////////////////////////// 	  

u8 Slave_Page = 1;
//外部中断初始化函数
void EXTIX_Init(void)
{
 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
 	
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PC端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PC端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//外部中断，需要使能AFIO时钟
	
	//重启中断
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化PC13
    GPIO_SetBits(GPIOA,GPIO_Pin_7);						    //PC13输高				
	 
	
/* 	//按钮S1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 			 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_12);
	
	//按钮s2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_13);
	
	
	//按钮s3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				 // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	
	//按钮s3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 // 端口配置
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化PC13
    GPIO_SetBits(GPIOB,GPIO_Pin_15); */
	
    //GPIOA.7 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line7;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

/* 	 //GPIOB.12 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource12);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	 //GPIOB.13 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line13;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	 //GPIOB.14 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource14);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line14;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	//GPIOB.15 中断线以及中断初始化配置
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource15);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器 */
	
 	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
		
/* 	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;	//抢占优先级0， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
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
			
			printf("重启\r\n");
			delay_ms(10);
			__set_FAULTMASK(1);
			NVIC_SystemReset();
		}
		
	
 	 EXTI_ClearITPendingBit(EXTI_Line7);    //清除LINE5上的中断标志位  
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
	
	EXTI_ClearITPendingBit(EXTI_Line12);		//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line13);		//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line14);		//清除中断标志位
	EXTI_ClearITPendingBit(EXTI_Line15);		//清除中断标志位
}
 */
