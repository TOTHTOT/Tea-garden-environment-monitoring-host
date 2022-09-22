//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F103RCT6,����ԭ��MiniSTM32������,��Ƶ72MHZ������12MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//wiki������վ��http://www.lcdwiki.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//����֧��QQ:3002773612  3002778157
//��������QQȺ:324828016
//��������:2018/08/09
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
//     LCDģ��                STM32��Ƭ��
//      VCC          ��        DC5V/3.3V      //��Դ
//      GND          ��          GND          //��Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������ΪSPI����
//     LCDģ��                STM32��Ƭ��    
//    SDI(MOSI)      ��          PB15         //Һ����SPI��������д�ź�
//    SDO(MISO)      ��          PB14         //Һ����SPI�������ݶ��źţ��������Ҫ�������Բ�����
//=======================================Һ���������߽���==========================================//
//     LCDģ�� 					      STM32��Ƭ�� 
//       LED         ��          PB9          //Һ������������źţ��������Ҫ���ƣ���5V��3.3V
//       SCK         ��          PB13         //Һ����SPI����ʱ���ź�
//      DC/RS        ��          PB10         //Һ��������/��������ź�
//       RST         ��          PB12         //Һ������λ�����ź�
//       CS          ��          PB11         //Һ����Ƭѡ�����ź�
//=========================================������������=========================================//
//���ģ�鲻���������ܻ��ߴ��д������ܣ����ǲ���Ҫ�������ܣ�����Ҫ���д���������
//	   LCDģ��                STM32��Ƭ�� 
//      T_IRQ        ��          PC10         //�����������ж��ź�
//      T_DO         ��          PC2          //������SPI���߶��ź�
//      T_DIN        ��          PC3          //������SPI����д�ź�
//      T_CS         ��          PC13         //������Ƭѡ�����ź�
//      T_CLK        ��          PC0          //������SPI����ʱ���ź�
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
u16 ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//������ɫ����
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
	//���ƹ̶���up
	LCD_Clear(WHITE);
//	delay_ms(1000);
	LCD_Fill(0,0,lcddev.width,20,BLUE);
	//���ƹ̶���down
	LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
	POINT_COLOR=WHITE;
	Gui_StrCenter(0,2,WHITE,BLUE,str,16,1);//������ʾ
	Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,now_time,16,1);//������ʾ
	//���Ʋ�������
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
	
	//��ʼ����
	PAGE = SLAVE_DATA_PAGE;
//	LCD_direction(1);
	DrawTestPage("��԰�������ϵͳ");
	//��ʾ�ӻ���ʶ
	Show_Str(20,30,BLUE,WHITE,"�ӻ�:1",16,0);
	Show_Str(5,50,BLUE,WHITE,"blml",16,0);	
	Show_Str(5,70,BLUE,WHITE,"xlsj",16,0);	
	Show_Str(5,90,BLUE,WHITE,"dqsj",16,0);


	mf_open("0:/ICON/WD.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(45,50,temp_pictur);
	Show_Str(WENDU_X,WENDU_Y,BLUE,YELLOW,"�¶�:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//���³�ʼ��
	
	mf_open("0:/ICON/SD.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(45,125,temp_pictur);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//���³�ʼ��
	Show_Str(SHIDU_X,SHIDU_Y,BLUE,YELLOW,"ʪ��:   ",16,1);
	
	mf_open("0:/ICON/CO2.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(125,50,temp_pictur);
	Show_Str(CO2_X,CO2_Y,BLUE,YELLOW,"CO2:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//���³�ʼ��

	mf_open("0:/ICON/TVOC.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(125,125,temp_pictur);
	Show_Str(130,175,BLUE,YELLOW,"TVOC:  ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//���³�ʼ��
	
	mf_open("0:/ICON/SD.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(205,50,temp_pictur);
	Show_Str(210,100,BLUE,YELLOW,"����ʪ��:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//���³�ʼ��
	
	mf_open("0:/ICON/GZ.BIN",1);
	mf_get_icon();
	mf_close();
	Gui_Drawbmp16(205,125,temp_pictur);
	Show_Str(210,175,BLUE,YELLOW,"����:   ",16,1);
	memset(temp_pictur, '\0', sizeof(temp_pictur));			//���³�ʼ��
	
	Show_Str(SHEZHI_X,SHEZHI_Y,BLUE,YELLOW,"����",16,1);
}

//��ʼ������ҳ��
void SheZhi_View()
{
	PAGE = SHEZHI_PAGE;					//����ҳ���ʶ
	
//	SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//���õ�����ģʽ
//	LCD_direction(1);					//������Ļ����
//	LCD_Clear(WHITE);
	DrawTestPage("����");				//��״̬��
//	delay_ms(50);
//	
	Get_config_Data();
	Show_Str(BACK_KEY_X,BACK_KEY_Y,BLUE,YELLOW,"����",16,1);
	Show_Str(SAVE_KEY_X,SAVE_KEY_Y,BLUE,YELLOW,"����",16, 1);
	
	Show_Str(SLAVE_QUANTITY_X,SLAVE_QUANTITY_Y,BLUE,YELLOW,"�ӻ�����:",16,1);  			    LCD_ShowNum(SLAVE_QUANTITY_X+85, SLAVE_QUANTITY_Y,slave_num, 3, 16);		
	Show_Str(SLAVE_ADDRESS_X,SLAVE_ADDRESS_Y,BLUE,YELLOW,"�ӻ���ַ:",16,1);				    LCD_ShowNum(SLAVE_ADDRESS_X+85,  SLAVE_ADDRESS_Y,Slave_channel_addr[0][0], 3, 16);
																							LCD_ShowNum(SLAVE_ADDRESS_X+125, SLAVE_ADDRESS_Y,Slave_channel_addr[0][1], 3, 16);
																							LCD_ShowNum(SLAVE_ADDRESS_X+165, SLAVE_ADDRESS_Y,Slave_channel_addr[0][2], 3, 16);
																							LCD_ShowNum(SLAVE_ADDRESS_X+205, SLAVE_ADDRESS_Y,Slave_channel_addr[0][3], 3, 16);	
	Show_Str(SCREEN_SLEEP_TIME_X,SCREEN_SLEEP_TIME_Y,BLUE,YELLOW,"����ʱ��:",16,1);		    LCD_ShowNum(SCREEN_SLEEP_TIME_X+85, SCREEN_SLEEP_TIME_Y,Screen_on_time, 3, 16);
	Show_Str(CALIBRATION_TIME_X,CALIBRATION_TIME_Y,BLUE,YELLOW,"У׼ʱ��",16,1);
	Show_Str(QUERY_INTERVAL_TIME_X,QUERY_INTERVAL_TIME_Y,BLUE,YELLOW,"��ѯ���:",16,1);	    LCD_ShowNum(QUERY_INTERVAL_TIME_X+85, QUERY_INTERVAL_TIME_Y,query_interval, 3, 16);
	LCD_ShowChar(PLUS_KEY_X, PLUS_KEY_Y, BLUE, WHITE, '+', 16, 0);
	LCD_ShowChar(MINUS_KEY_X, MINUS_KEY_Y, BLUE, WHITE, '-', 16, 0);
	
}

void Load_Keyboard( )
{
	
}



