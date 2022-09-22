//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STM32F103RCT6,正点原子MiniSTM32开发板,主频72MHZ，晶振12MHZ
//QDtech-TFT液晶驱动 for STM32 IO模拟
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtft.com
//淘宝网站：http://qdtech.taobao.com
//wiki技术网站：http://www.lcdwiki.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//技术支持QQ:3002773612  3002778157
//技术交流QQ群:324828016
//创建日期:2018/08/09
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================电源接线================================================//
//     LCD模块                STM32单片机
//      VCC          接        DC5V/3.3V      //电源
//      GND          接          GND          //电源地
//=======================================液晶屏数据线接线==========================================//
//本模块默认数据总线类型为SPI总线
//     LCD模块                STM32单片机    
//    SDI(MOSI)      接          PB15         //液晶屏SPI总线数据写信号
//    SDO(MISO)      接          PB14         //液晶屏SPI总线数据读信号，如果不需要读，可以不接线
//=======================================液晶屏控制线接线==========================================//
//     LCD模块 					      STM32单片机 
//       LED         接          PB9          //液晶屏背光控制信号，如果不需要控制，接5V或3.3V
//       SCK         接          PB13         //液晶屏SPI总线时钟信号
//      DC/RS        接          PB10         //液晶屏数据/命令控制信号
//       RST         接          PB12         //液晶屏复位控制信号
//       CS          接          PB11         //液晶屏片选控制信号
//=========================================触摸屏触接线=========================================//
//如果模块不带触摸功能或者带有触摸功能，但是不需要触摸功能，则不需要进行触摸屏接线
//	   LCD模块                STM32单片机 
//      T_IRQ        接          PC10         //触摸屏触摸中断信号
//      T_DO         接          PC2          //触摸屏SPI总线读信号
//      T_DIN        接          PC3          //触摸屏SPI总线写信号
//      T_CS         接          PC13         //触摸屏片选控制信号
//      T_CLK        接          PC0          //触摸屏SPI总线时钟信号
**************************************************************************************************/	
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "lcd.h"
#include "delay.h"
#include "gui.h"
#include "test.h"
#include "touch.h"
#include "led.h"
#include "pic.h"
#include "rtc.h"
#include "usart.h"
#include "fattester.h"	
#include "main.h"
#include "spi.h"
#include "24cxx.h"

//========================variable==========================//
u16 ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//定义颜色数组
//=====================end of variable======================//
u8 temp_pictur[3200];
/*****************************************************************************
 * @name       :void DrawTestPage(u8 *str)
 * @date       :2018-08-09 
 * @function   :Drawing test interface
 * @parameters :str:the start address of the Chinese and English Fstrings
 * @retvalue   :None
******************************************************************************/ 
void DrawTestPage(u8 *str)
{
	//绘制固定栏up
	LCD_Clear(WHITE);
//	delay_ms(1000);
	LCD_Fill(0,0,lcddev.width,20,BLUE);
	//绘制固定栏down
	LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
	POINT_COLOR=WHITE;
	Gui_StrCenter(0,2,WHITE,BLUE,str,16,1);//居中显示
	Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,now_time,16,1);//居中显示
	//绘制测试区域
	//LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
}

/*****************************************************************************
 * @name       :void Rotate_Test(void)
 * @date       :2018-08-09 
 * @function   :rotate test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Rotate_Test(void)
{
	u8 *Direction[4]={"Rotation:0","Rotation:90","Rotation:180","Rotation:270"};
	
	//初始界面
	PAGE = SLAVE_DATA_PAGE;
//	LCD_direction(1);
	DrawTestPage("茶园环境监测系统");
	//显示从机标识
	Show_Str(20,30,BLUE,WHITE,"从机:1",16,0);
	Show_Str(5,50,BLUE,WHITE,"blml",16,0);	
	Show_Str(5,70,BLUE,WHITE,"xlsj",16,0);	
	Show_Str(5,90,BLUE,WHITE,"dqsj",16,0);


	mf_open("0:/ICON/WD.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(45,50,temp_pictur);
	Show_Str(WENDU_X,WENDU_Y,BLUE,YELLOW,"温度:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//重新初始化
	
	mf_open("0:/ICON/SD.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(45,125,temp_pictur);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//重新初始化
	Show_Str(SHIDU_X,SHIDU_Y,BLUE,YELLOW,"湿度:   ",16,1);
	
	mf_open("0:/ICON/CO2.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(125,50,temp_pictur);
	Show_Str(CO2_X,CO2_Y,BLUE,YELLOW,"CO2:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//重新初始化

	mf_open("0:/ICON/TVOC.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(125,125,temp_pictur);
	Show_Str(130,175,BLUE,YELLOW,"TVOC:  ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//重新初始化
	
	mf_open("0:/ICON/SD.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(205,50,temp_pictur);
	Show_Str(210,100,BLUE,YELLOW,"土壤湿度:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//重新初始化
	
	mf_open("0:/ICON/GZ.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(205,125,temp_pictur);
	Show_Str(210,175,BLUE,YELLOW,"光照:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//重新初始化
	
	Show_Str(SHEZHI_X,SHEZHI_Y,BLUE,YELLOW,"设置",16,1);
}

//初始化设置页面
void SheZhi_View()
{
	PAGE = SHEZHI_PAGE;					//设置页面标识
	
//	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//设置到高速模式
//	LCD_direction(1);					//设置屏幕方向
//	LCD_Clear(WHITE);
	DrawTestPage("设置");				//画状态栏
//	delay_ms(50);
//	
	Get_config_Data();
	Show_Str(BACK_KEY_X,BACK_KEY_Y,BLUE,YELLOW,"返回",16,1);
	Show_Str(SAVE_KEY_X,SAVE_KEY_Y,BLUE,YELLOW,"保存",16, 1);
	
	Show_Str(SLAVE_QUANTITY_X,SLAVE_QUANTITY_Y,BLUE,YELLOW,"从机数量:",16,1);  			    LCD_ShowNum(SLAVE_QUANTITY_X+85, SLAVE_QUANTITY_Y,slave_num, 3, 16);		
	Show_Str(SLAVE_ADDRESS_X,SLAVE_ADDRESS_Y,BLUE,YELLOW,"从机地址:",16,1);				    LCD_ShowNum(SLAVE_ADDRESS_X+85,  SLAVE_ADDRESS_Y,Slave_channel_addr[0][0], 3, 16);
																							LCD_ShowNum(SLAVE_ADDRESS_X+125, SLAVE_ADDRESS_Y,Slave_channel_addr[0][1], 3, 16);
																							LCD_ShowNum(SLAVE_ADDRESS_X+165, SLAVE_ADDRESS_Y,Slave_channel_addr[0][2], 3, 16);
																							LCD_ShowNum(SLAVE_ADDRESS_X+205, SLAVE_ADDRESS_Y,Slave_channel_addr[0][3], 3, 16);	
	Show_Str(SCREEN_SLEEP_TIME_X,SCREEN_SLEEP_TIME_Y,BLUE,YELLOW,"休眠时间:",16,1);		    LCD_ShowNum(SCREEN_SLEEP_TIME_X+85, SCREEN_SLEEP_TIME_Y,Screen_on_time, 3, 16);
	Show_Str(CALIBRATION_TIME_X,CALIBRATION_TIME_Y,BLUE,YELLOW,"校准时间",16,1);
	Show_Str(QUERY_INTERVAL_TIME_X,QUERY_INTERVAL_TIME_Y,BLUE,YELLOW,"轮询间隔:",16,1);	    LCD_ShowNum(QUERY_INTERVAL_TIME_X+85, QUERY_INTERVAL_TIME_Y,query_interval, 3, 16);
	LCD_ShowChar(PLUS_KEY_X, PLUS_KEY_Y, BLUE, WHITE, '+', 16, 0);
	LCD_ShowChar(MINUS_KEY_X, MINUS_KEY_Y, BLUE, WHITE, '-', 16, 0);
	
}

void Load_Keyboard( )
{
	
}



