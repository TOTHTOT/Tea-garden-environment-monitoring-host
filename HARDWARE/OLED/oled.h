#ifndef __OLED_H
#define __OLED_H			  	 

#include "delay.h"
#include "sys.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "sys.h"
 
void IIC_Init(void);
void IIC_ACK(void);
void IIC_NACK(void);
u8 IIC_wait_ACK(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendByte(u8 byte);
u8 IIC_RcvByte(void);





#define OLED_ON  0xFF	//亮
#define OLED_OFF 0x00	//灭
#define high 1
#define low 0
#define	Brightness	0xCF 
#define X_WIDTH 	128
#define Y_WIDTH 	64
 
/*OLED IIC通讯相关宏定义*/
#define OLED_CMD 				0		//写命令
#define OLED_DATA 				1		//写数据
#define IIC_ADD_WC 				0x78	//IIC通讯写命令时的地址
#define IIC_ADD_WD 				0x78	//IIC通讯写数据时的地址
#define IIC_CONC 				0x02	//控制字节指定下一个字节为命令
#define IIC_COND 				0x40	//控制字节指定下一个字节为数据SS
 
/*SSD1306 基础命令定义*/
#define SET_CONTRAST_CONTROL		        0x81	//对比度设置命令，取值范围1-255，复位值：0x7F
#define SET_DISPLAY_ON				0xAF	//开显示命令
#define SET_DISPLAY_OFF				0xAE	//关显示命令
#define ENTIRE_DISPLAY_ON_A4		        0xA4	//全局显示关闭
#define ENTIRE_DISPLAY_ON_A5		        0xA5	//全局显示开启
#define NORMAL_DISPLAY				0xA6	//给GDDRAM值为0，熄灭像素；给GDDRAM值为1，点亮像素
#define INVERSE_DISPLAY				0xA7	//给GDDRAM值为0，点亮像素；给GDDRAM值为1，熄灭像素
 
/*SSD1306 滚动显示命令定义*/
#define HORIZONTAL_SCROLL_SETUP_RIGHT	        0x26	//向右水平移动
#define HORIZONTAL_SCROLL_SETUP_LEFT	        0x27	//向左水平移动
#define HORIZONTAL_SCROLL_SETUP_AE		0x00	//配置水平移动时，需要的虚拟字节
#define HORIZONTAL_SCROLL_SETUP_F		0xFF	//配置水平移动时，需要的虚拟字节
#define ACTIVATE_SCROLL				0x2F	//开启滚动显示功能	
#define DEACTIVATE_SCROLL			0x2E	//关闭滚动显示功能
#define SET_VERTICAL_SCROLL_AREA		0xA3	//设置垂直滚动范围命令	
#define HV_SCROLL_SRTEP_RIGHT			0x29	//向右水平移动
#define HV_SCROLL_SRTEP_LEFT			0x2A	//向左水平移动
 
/*SSD1306 地址设置命令定义*/
#define SET_MEMORY_ADDR_MODE			0x20	//设置地址模式：0x00,列地址模式；0x01,行地址模式；0x10，页地址模式
 
/*SSD1306 硬件配置命令定义*/
#define SET_SEGMENT_REMAP_A0			0xA0	//列地址0映射到SEG0
#define SET_SEGMENT_REMAP_A1			0xA1	//列地址0映射到SEG127
#define SET_MULTIPLEX_RATIO			0xA8	//设置驱动路数
#define SET_COM_SCAN_DIRECTION_C0		0xC0	//扫描方向从COM0-->COM[N-1]
#define SET_COM_SCAN_DIRECTION_C8		0xC8	//扫描方向从COM[N-1]-->COM0
#define SET_DISPLAY_OFFSET			0xD3	//设置行显示偏移
#define SET_COM_PINS_CONFIG			0xDA	//设置COM硬件引脚配置命令
 
/*SSD1306 显示时钟和驱动相关命令定义*/
#define SET_DISPLAY_CLOCK			0xD5	//设置时钟分频因子、振荡频率
#define SET_PRECHARGE_PERIOD			0xD9	//设置预充电周期
#define SET_VCOMH_DESELECT_L			0xDB	//设置VCOMH电源倍率
#define CHARGE_PUMP_SET				0x8D	//电荷泵设置
 
/*显示字符类型定义*/
#define TYPE_12X06			        0x01	//显示12*06字符
#define TYPE_16X08			        0x02	//显示16*08字符
#define TYPE_16X16			        0x03	//显示16*16字符
 
extern u8 Old_Menu_Page;
extern u8 Display_Fleg;
extern u8 OLED_GRAM[8][128];
extern u8 Menu_Page;
extern u8 Display_Fleg;
extern u8 Data_buff[25];
 

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size);
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void); 
void OLED_Init(void);
void OLED_Draw_Point(u8 x,u8 y,u8 c);
void OLED_Refresh_Screen(void);
void LCD_ShowFont(u32 x,u32 y,u32 w,u32 h,u8 *p);
void OLED_SetCursorAddrese(u8 x,u8 y);
void OLED_PageClear(u8 page);
void OLED_DisplayString(u8 x,u8 y,u8 width,u8 height,u8 *str);
u8 Oled_Display(u8 x ,u8 y ,u8 width , u8 height);
void OLED_Menu_Display(void);
 

#endif  
	 



