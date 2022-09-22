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
#include "touch.h" 
#include "lcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
#include "24cxx.h"
#include "gui.h"	    
#include "usart.h"
#include "test.h"
#include "main.h"
#include "timer.h"

_m_tp_dev tp_dev=
{
	TP_Init,
	TP_Scan,
	TP_Adjust,
	0,
	0,
 	0,
	0,
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};					
//Ĭ��Ϊtouchtype=0������.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

u8 cursor_pos = 0;
/*****************************************************************************
 * @name       :void TP_Write_Byte(u8 num)   
 * @date       :2018-08-09 
 * @function   :Write a byte data to the touch screen IC with SPI bus
 * @parameters :num:Data to be written
 * @retvalue   :None
******************************************************************************/  	 			    					   
void TP_Write_Byte(u8 num)    
{  
	u8 count=0;   
	for(count=0;count<8;count++)  
	{ 	  
		if(num&0x80)TDIN=1;  
		else TDIN=0;   
		num<<=1;    
		TCLK=0; 	 
		TCLK=1;		//��������Ч	        
	}		 			    
}

/*****************************************************************************
 * @name       :u16 TP_Read_AD(u8 CMD)	  
 * @date       :2018-08-09 
 * @function   :Reading adc values from touch screen IC with SPI bus
 * @parameters :CMD:Read command,0xD0 for x,0x90 for y
 * @retvalue   :Read data
******************************************************************************/    
u16 TP_Read_AD(u8 CMD)	  
{ 	 
	u8 count=0; 	  
	u16 Num=0; 
	TCLK=0;		//������ʱ�� 	 
	TDIN=0; 	//����������
	TCS=0; 		//ѡ�д�����IC
	TP_Write_Byte(CMD);//����������
	delay_us(6);//ADS7846��ת��ʱ���Ϊ6us
	TCLK=0; 	     	    
	delay_us(1);    	   
	TCLK=1;		//��1��ʱ�ӣ����BUSY	    	    
	TCLK=0; 	     	    
	for(count=0;count<16;count++)//����16λ����,ֻ�и�12λ��Ч 
	{ 				  
		Num<<=1; 	 
		TCLK=0;	//�½�����Ч  	    	   
		TCLK=1;
		if(DOUT)Num++; 		 
	}  	
	Num>>=4;   	//ֻ�и�12λ��Ч.
	TCS=1;		//�ͷ�Ƭѡ	 
	return(Num);  
//#endif
}

#define READ_TIMES 5 	//��ȡ����
#define LOST_VAL 1	  	//����ֵ
/*****************************************************************************
 * @name       :u16 TP_Read_XOY(u8 xy)  
 * @date       :2018-08-09 
 * @function   :Read the touch screen coordinates (x or y),
								Read the READ_TIMES secondary data in succession 
								and sort the data in ascending order,
								Then remove the lowest and highest number of LOST_VAL 
								and take the average
 * @parameters :xy:Read command(CMD_RDX/CMD_RDY)
 * @retvalue   :Read data
******************************************************************************/  
u16 TP_Read_XOY(u8 xy)
{
	u16 i, j;
	u16 buf[READ_TIMES];
	u16 sum=0;
	u16 temp;
	for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);		 		    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

/*****************************************************************************
 * @name       :u8 TP_Read_XY(u16 *x,u16 *y)
 * @date       :2018-08-09 
 * @function   :Read touch screen x and y coordinates,
								The minimum value can not be less than 100
 * @parameters :x:Read x coordinate of the touch screen
								y:Read y coordinate of the touch screen
 * @retvalue   :0-fail,1-success
******************************************************************************/ 
u8 TP_Read_XY(u16 *x,u16 *y)
{
	u16 xtemp,ytemp;			 	 		  
	xtemp=TP_Read_XOY(CMD_RDX);
	ytemp=TP_Read_XOY(CMD_RDY);	  												   
	if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}

#define ERR_RANGE 50 //��Χ 
/*****************************************************************************
 * @name       :u8 TP_Read_XY2(u16 *x,u16 *y) 
 * @date       :2018-08-09 
 * @function   :Read the touch screen coordinates twice in a row, 
								and the deviation of these two times can not exceed ERR_RANGE, 
								satisfy the condition, then think the reading is correct, 
								otherwise the reading is wrong.
								This function can greatly improve the accuracy.
 * @parameters :x:Read x coordinate of the touch screen
								y:Read y coordinate of the touch screen
 * @retvalue   :0-fail,1-success
******************************************************************************/ 
u8 TP_Read_XY2(u16 *x,u16 *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=TP_Read_XY(&x1,&y1);   
    if(flag==0)return(0);
    flag=TP_Read_XY(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 

/*****************************************************************************
 * @name       :void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
 * @date       :2018-08-09 
 * @function   :Draw a touch point,Used to calibrate							
 * @parameters :x:Read x coordinate of the touch screen
								y:Read y coordinate of the touch screen
								color:the color value of the touch point
 * @retvalue   :None
******************************************************************************/  
void TP_Drow_Touch_Point(u16 x,u16 y,u16 color)
{
	POINT_COLOR=color;
	LCD_DrawLine(x-12,y,x+13,y);//����
	LCD_DrawLine(x,y-12,x,y+13);//����
	LCD_DrawPoint(x+1,y+1);
	LCD_DrawPoint(x-1,y+1);
	LCD_DrawPoint(x+1,y-1);
	LCD_DrawPoint(x-1,y-1);
	gui_circle(x,y,POINT_COLOR,6,0);//������Ȧ
}	

/*****************************************************************************
 * @name       :void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
 * @date       :2018-08-09 
 * @function   :Draw a big point(2*2)					
 * @parameters :x:Read x coordinate of the point
								y:Read y coordinate of the point
								color:the color value of the point
 * @retvalue   :None
******************************************************************************/   
void TP_Draw_Big_Point(u16 x,u16 y,u16 color)
{	    
	POINT_COLOR=color;
	LCD_DrawPoint(x,y);//���ĵ� 
	LCD_DrawPoint(x+1,y);
	LCD_DrawPoint(x,y+1);
	LCD_DrawPoint(x+1,y+1);	 	  	
}	

/*****************************************************************************
 * @name       :u8 TP_Scan(u8 tp)
 * @date       :2018-08-09 
 * @function   :Scanning touch event				
 * @parameters :tp:0-screen coordinate 
									 1-Physical coordinates(For special occasions such as calibration)
 * @retvalue   :Current touch screen status,
								0-no touch
								1-touch
******************************************************************************/  					  
u8 TP_Scan(u8 tp)
{			   
	if(PEN==0)//�а�������
	{
		if(tp)TP_Read_XY2(&tp_dev.x,&tp_dev.y);//��ȡ��������
		else if(TP_Read_XY2(&tp_dev.x,&tp_dev.y))//��ȡ��Ļ����
		{
	 		tp_dev.x=tp_dev.xfac*tp_dev.x+tp_dev.xoff;//�����ת��Ϊ��Ļ����
			tp_dev.y=tp_dev.yfac*tp_dev.y+tp_dev.yoff;  
	 	} 
		if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
		{		 
			tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
			tp_dev.x0=tp_dev.x;//��¼��һ�ΰ���ʱ������
			tp_dev.y0=tp_dev.y;  	   			 
		}			   
	}else
	{
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x0=0;
			tp_dev.y0=0;
			tp_dev.x=0xffff;
			tp_dev.y=0xffff;
		}	    
	}
	// printf("x:%d y:%d\r\n", );
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
}
	  
//////////////////////////////////////////////////////////////////////////	 
//������EEPROM����ĵ�ַ�����ַ,ռ��13���ֽ�(RANGE:SAVE_ADDR_BASE~SAVE_ADDR_BASE+12)
#define SAVE_ADDR_BASE 40
/*****************************************************************************
 * @name       :void TP_Save_Adjdata(void)
 * @date       :2018-08-09 
 * @function   :Save calibration parameters		
 * @parameters :None
 * @retvalue   :None
******************************************************************************/ 										    
void TP_Save_Adjdata(void)
{
	s32 temp;			 
	//����У�����!		   							  
	temp=tp_dev.xfac*100000000;//����xУ������      
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE,temp,4);   
	temp=tp_dev.yfac*100000000;//����yУ������    
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+4,temp,4);
	//����xƫ����
    AT24CXX_WriteLenByte(SAVE_ADDR_BASE+8,tp_dev.xoff,2);		    
	//����yƫ����
	AT24CXX_WriteLenByte(SAVE_ADDR_BASE+10,tp_dev.yoff,2);	
	//���津������
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+12,tp_dev.touchtype);	
	temp=0X0A;//���У׼����
	AT24CXX_WriteOneByte(SAVE_ADDR_BASE+13,temp); 
}

/*****************************************************************************
 * @name       :u8 TP_Get_Adjdata(void)
 * @date       :2018-08-09 
 * @function   :Gets the calibration values stored in the EEPROM		
 * @parameters :None
 * @retvalue   :1-get the calibration values successfully
								0-get the calibration values unsuccessfully and Need to recalibrate
******************************************************************************/ 	
u8 TP_Get_Adjdata(void)
{					  
	s32 tempfac;
	tempfac=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+13);//��ȡ�����,���Ƿ�У׼���� 		 
	if(tempfac==0X0A)//�������Ѿ�У׼����			   
	{    												 
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE,4);		   
		tp_dev.xfac=(float)tempfac/100000000;//�õ�xУ׼����
		tempfac=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+4,4);			          
		tp_dev.yfac=(float)tempfac/100000000;//�õ�yУ׼����
	    //�õ�xƫ����
		tp_dev.xoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+8,2);			   	  
 	    //�õ�yƫ����
		tp_dev.yoff=AT24CXX_ReadLenByte(SAVE_ADDR_BASE+10,2);				 	  
 		tp_dev.touchtype=AT24CXX_ReadOneByte(SAVE_ADDR_BASE+12);//��ȡ�������ͱ��
		if(tp_dev.touchtype)//X,Y��������Ļ�෴
		{
			CMD_RDX=0X90;
			CMD_RDY=0XD0;	 
		}else				   //X,Y��������Ļ��ͬ
		{
			CMD_RDX=0XD0;
			CMD_RDY=0X90;	 
		}		 
		return 1;	 
	}
	return 0;
}	
 
//��ʾ�ַ���
const u8* TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";

/*****************************************************************************
 * @name       :void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
 * @date       :2018-08-09 
 * @function   :Display calibration results	
 * @parameters :x0:the x coordinates of first calibration point
								y0:the y coordinates of first calibration point
								x1:the x coordinates of second calibration point
								y1:the y coordinates of second calibration point
								x2:the x coordinates of third calibration point
								y2:the y coordinates of third calibration point
								x3:the x coordinates of fourth calibration point
								y3:the y coordinates of fourth calibration point
								fac:calibration factor 
 * @retvalue   :None
******************************************************************************/ 	 					  
void TP_Adj_Info_Show(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2,u16 x3,u16 y3,u16 fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,140,16,"x1:",1);
 	LCD_ShowString(40+80,140,16,"y1:",1);
 	LCD_ShowString(40,160,16,"x2:",1);
 	LCD_ShowString(40+80,160, 16,"y2:",1);
	LCD_ShowString(40,180, 16,"x3:",1);
 	LCD_ShowString(40+80,180, 16,"y3:",1);
	LCD_ShowString(40,200, 16,"x4:",1);
 	LCD_ShowString(40+80,200, 16,"y4:",1);  
 	LCD_ShowString(40,220, 16,"fac is:",1);     
	LCD_ShowNum(40+24,140,x0,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,140,y0,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,160,x1,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,160,y1,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,180,x2,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,180,y2,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,200,x3,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,200,y3,4,16);	//��ʾ��ֵ
 	LCD_ShowNum(40+56,220,fac,3,16); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.
}

/*****************************************************************************
 * @name       :u8 TP_Get_Adjdata(void)
 * @date       :2018-08-09 
 * @function   :Calibration touch screen and Get 4 calibration parameters
 * @parameters :None
 * @retvalue   :None
******************************************************************************/ 		 
void TP_Adjust(void)
{								 
	u16 pos_temp[4][2];//���껺��ֵ
	u8  cnt=0;	
	u16 d1,d2;
	u32 tem1,tem2;
	float fac; 	
	u16 outtime=0;
 	cnt=0;				
	POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//����   
	POINT_COLOR=RED;//��ɫ 
	LCD_Clear(WHITE);//���� 	   
	POINT_COLOR=BLACK;
	LCD_ShowString(10,40,16,"Please use the stylus click",1);//��ʾ��ʾ��Ϣ
	LCD_ShowString(10,56,16,"the cross on the screen.",1);//��ʾ��ʾ��Ϣ
	LCD_ShowString(10,72,16,"The cross will always move",1);//��ʾ��ʾ��Ϣ
	LCD_ShowString(10,88,16,"until the screen adjustment",1);//��ʾ��ʾ��Ϣ
  LCD_ShowString(10,104,16,"is completed.",1);//��ʾ��ʾ��Ϣ
	 
	TP_Drow_Touch_Point(20,20,RED);//����1 
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)//�������10����û�а���,���Զ��˳�
	{
		tp_dev.scan(1);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x;
			pos_temp[cnt][1]=tp_dev.y;
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,WHITE);				//�����1 
					TP_Drow_Touch_Point(lcddev.width-20,20,RED);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(lcddev.width-20,20,WHITE);	//�����2
					TP_Drow_Touch_Point(20,lcddev.height-20,RED);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,lcddev.height-20,WHITE);			//�����3
 					TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
 						TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
					//������
					tp_dev.xfac=(float)(lcddev.width-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					tp_dev.xoff=(lcddev.width-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					tp_dev.yfac=(float)(lcddev.height-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					tp_dev.yoff=(lcddev.height-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//������Ԥ����෴��.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(lcddev.width-20,lcddev.height-20,WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,RED);								//����1
						LCD_ShowString(40,26, 16,"TP Need readjust!",1);
						tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
						if(tp_dev.touchtype)//X,Y��������Ļ�෴
						{
							CMD_RDX=0X90;
							CMD_RDY=0XD0;	 
						}else				   //X,Y��������Ļ��ͬ
						{
							CMD_RDX=0XD0;
							CMD_RDY=0X90;	 
						}			    
						continue;
					}		
					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//����
					LCD_ShowString(35,110, 16,"Touch Screen Adjust OK!",1);//У�����
					delay_ms(1000);
					TP_Save_Adjdata();  
 					LCD_Clear(WHITE);//����   
					return;//У�����				 
			}
		}
		delay_ms(10);
		outtime++;
		if(outtime>10000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
 	}
}		

/*****************************************************************************
 * @name       :u8 TP_Init(void)
 * @date       :2018-08-09 
 * @function   :Initialization touch screen
 * @parameters :None
 * @retvalue   :0-no calibration
								1-Has been calibrated
******************************************************************************/  
u8 TP_Init(void)
{			    		   
	//ע��,ʱ��ʹ��֮��,��GPIO�Ĳ�������Ч
	//��������֮ǰ,����ʹ��ʱ��.����ʵ���������������
	GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
	EXTI_InitTypeDef EXTI_InitStructure;    //�ж���
	NVIC_InitTypeDef NVIC_InitStructure;	//�жϳ�ʼ��


	
	//ע��,ʱ��ʹ��֮��,��GPIO�Ĳ�������Ч
	//��������֮ǰ,����ʹ��ʱ��.����ʵ���������������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;  //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	   
	   
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//��ռ���ȼ�2 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	
  	TP_Read_XY(&tp_dev.x,&tp_dev.y);//��һ�ζ�ȡ��ʼ��	 
 	AT24CXX_Init();//��ʼ��24CXX
	if(TP_Get_Adjdata())return 0;//�Ѿ�У׼
	else			   //δУ׼?
	{ 										    
		LCD_Clear(WHITE);//����
	    TP_Adjust();  //��ĻУ׼ 
		TP_Save_Adjdata();	 
	}			
	TP_Get_Adjdata();	
	return 1; 									 
}

void EXTI4_IRQHandler(void)
{
	static u8 flag_num = 0;
	u8 i, j, temp_data = 0;
	delay_ms(10);
	tp_dev.scan(0); 	
	delay_ms(10);
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{
		Control_screen_on_off(1);
		//�ж��Ƿ��ڵ�1ҳ���������
		if((tp_dev.x<SHEZHI_X +40&&tp_dev.x>SHEZHI_X)&&(tp_dev.y>SHEZHI_Y&&tp_dev.y<SHEZHI_Y+20)&&PAGE == SLAVE_DATA_PAGE)
		{
			SheZhi_View();
		}
		//�ж��Ƿ��ڵ�2ҳ����˷���
		if((tp_dev.x<BACK_KEY_X +40&&tp_dev.x>BACK_KEY_X)&&(tp_dev.y>BACK_KEY_Y&&tp_dev.y<BACK_KEY_Y+20)&&PAGE == SHEZHI_PAGE)
		{
			Rotate_Test();
		}
		//�ж��Ƿ��ڵ�2ҳ����˱���
		if((tp_dev.x<SAVE_KEY_X +40&&tp_dev.x>SAVE_KEY_X)&&(tp_dev.y>SAVE_KEY_Y&&tp_dev.y<SAVE_KEY_Y+20)&&PAGE == SHEZHI_PAGE)
		{
			Show_Str(160,200,BLUE,WHITE,"������!!!",16,0);
			//д�����ݵ�AT24C02
			AT24CXX_WriteLenByte(AT24CXX_SLAVE_NUM_ADDR, slave_num, 1);
			AT24CXX_WriteLenByte(AT24CXX_MASTER_QUERY_INTERVAL_TIME, query_interval, 1);
			AT24CXX_WriteLenByte(AT24CXX_MASTER_SCREEN_ON_TIME, Screen_on_time, 1);
			for(i = 0; i < slave_num; i++)
			{
				for(j = 0; j < 4; j++)
				{
					//temp_data ++;
					//AT24CXX_WriteOneByte(AT24CXX_SLAVE_ADDR_ADDR+(i*4)+j, i*2);
					AT24CXX_WriteOneByte(AT24CXX_SLAVE_ADDR_ADDR+(i*4)+j, Slave_channel_addr[i][j]);
					//delay_ms(1);
					//printf("addr:%d,temp:%d ", AT24CXX_SLAVE_ADDR_ADDR+(i*4)+j, temp_data);
				}
				//printf("\r\n");
			}
			Show_Str(160,200,BLUE,WHITE,"�������!!",16,0);
			delay_ms(500);
			Show_Str(160,200,BLUE,WHITE,"          ",16,0);
			cursor_pos = 0;
			flag_num = 0;
			Rotate_Test();										//������ɺ󷵻�������
		}
		//�ж��Ƿ��ڵ�2ҳ����ʾ���λ��(�޸Ĵӻ�����)
		if((tp_dev.x<SLAVE_QUANTITY_X +90&&tp_dev.x>SLAVE_QUANTITY_X)&&(tp_dev.y>SLAVE_QUANTITY_Y-5&&tp_dev.y<SLAVE_QUANTITY_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			cursor_pos = 1;
			LCD_ShowChar(SLAVE_ADDRESS_X+72,  SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ���Ź����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+112, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ߵ�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+152, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��͵�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+192, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ŵ������ʾ
			LCD_ShowChar(SLAVE_QUANTITY_X+72, SLAVE_QUANTITY_Y, BLUE, WHITE, '~', 16, 0);						//�ӻ����������ʾ
			LCD_ShowChar(SCREEN_SLEEP_TIME_X+72, SCREEN_SLEEP_TIME_Y, BLUE, WHITE, ' ', 16, 0);					//��Ļ����ʱ������ʾ
			LCD_ShowChar(QUERY_INTERVAL_TIME_X+72, QUERY_INTERVAL_TIME_Y, BLUE, WHITE, ' ', 16, 0);				//��ѯ�������ʾ
		}
		//�ж��Ƿ��ڵ�2ҳ����ʾ���λ��(�޸�����ʱ��)
		if((tp_dev.x<SCREEN_SLEEP_TIME_X +90&&tp_dev.x>SCREEN_SLEEP_TIME_X)&&(tp_dev.y>SCREEN_SLEEP_TIME_Y-10&&tp_dev.y<SCREEN_SLEEP_TIME_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			cursor_pos = 2;
			LCD_ShowChar(SLAVE_ADDRESS_X+72,  SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ���Ź����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+112, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ߵ�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+152, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��͵�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+192, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ŵ������ʾ
			LCD_ShowChar(SLAVE_QUANTITY_X+72, SLAVE_QUANTITY_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ����������ʾ
			LCD_ShowChar(SCREEN_SLEEP_TIME_X+72, SCREEN_SLEEP_TIME_Y, BLUE, WHITE, '~', 16, 0);					//��Ļ����ʱ������ʾ
			LCD_ShowChar(QUERY_INTERVAL_TIME_X+72, QUERY_INTERVAL_TIME_Y, BLUE, WHITE, ' ', 16, 0);				//��ѯ�������ʾ
		}
		//�ж��Ƿ��ڵ�2ҳ����ʾ���λ��(�޸���ѯ���)
		if((tp_dev.x<QUERY_INTERVAL_TIME_X +90&&tp_dev.x>QUERY_INTERVAL_TIME_X)&&(tp_dev.y>QUERY_INTERVAL_TIME_Y-10&&tp_dev.y<QUERY_INTERVAL_TIME_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			cursor_pos = 3;
			LCD_ShowChar(SLAVE_ADDRESS_X+72,  SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ���Ź����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+112, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ߵ�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+152, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��͵�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+192, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ŵ������ʾ
			LCD_ShowChar(SLAVE_QUANTITY_X+72, SLAVE_QUANTITY_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ����������ʾ
			LCD_ShowChar(SCREEN_SLEEP_TIME_X+72, SCREEN_SLEEP_TIME_Y, BLUE, WHITE, ' ', 16, 0);					//��Ļ����ʱ������ʾ
			LCD_ShowChar(QUERY_INTERVAL_TIME_X+72, QUERY_INTERVAL_TIME_Y, BLUE, WHITE, '~', 16, 0);				//��ѯ�������ʾ
		}
		//�ж��Ƿ��ڵ�2ҳ����ʾ���λ��(�޸Ĵӻ����)
		if((tp_dev.x<SLAVE_ADDRESS_X +124&&tp_dev.x>SLAVE_ADDRESS_X+85)&&(tp_dev.y>SLAVE_ADDRESS_Y-10&&tp_dev.y<SLAVE_ADDRESS_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			cursor_pos = 4;
			LCD_ShowChar(SLAVE_ADDRESS_X+72,  SLAVE_ADDRESS_Y, BLUE, WHITE, '~', 16, 0);						//�ӻ���Ź����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+112, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ߵ�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+152, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��͵�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+192, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ��ŵ������ʾ
			LCD_ShowChar(SLAVE_QUANTITY_X+72, SLAVE_QUANTITY_Y, BLUE, WHITE, ' ', 16, 0);						//�ӻ����������ʾ
			LCD_ShowChar(SCREEN_SLEEP_TIME_X+72, SCREEN_SLEEP_TIME_Y, BLUE, WHITE, ' ', 16, 0);					//��Ļ����ʱ������ʾ
			LCD_ShowChar(QUERY_INTERVAL_TIME_X+72, QUERY_INTERVAL_TIME_Y, BLUE, WHITE, ' ', 16, 0);				//��ѯ�������ʾ
		}
		//�ж��Ƿ��ڵ�2ҳ����ʾ���λ��(�޸Ĵӻ��ߵ�ַ)
		if((tp_dev.x<SLAVE_ADDRESS_X +164&&tp_dev.x>SLAVE_ADDRESS_X+125)&&(tp_dev.y>SLAVE_ADDRESS_Y-10&&tp_dev.y<SLAVE_ADDRESS_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			cursor_pos = 5;
			LCD_ShowChar(SLAVE_ADDRESS_X+72,  SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ���Ź����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+112, SLAVE_ADDRESS_Y, BLUE, WHITE, '~', 16, 0);							//�ӻ��ߵ�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+152, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ��͵�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+192, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ��ŵ������ʾ
			LCD_ShowChar(SLAVE_QUANTITY_X+72, SLAVE_QUANTITY_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ����������ʾ
			LCD_ShowChar(SCREEN_SLEEP_TIME_X+72, SCREEN_SLEEP_TIME_Y, BLUE, WHITE, ' ', 16, 0);						//��Ļ����ʱ������ʾ
			LCD_ShowChar(QUERY_INTERVAL_TIME_X+72, QUERY_INTERVAL_TIME_Y, BLUE, WHITE, ' ', 16, 0);					//��ѯ�������ʾ
		}
		//�ж��Ƿ��ڵ�2ҳ����ʾ���λ��(�޸Ĵӵ͵�ַ)
		if((tp_dev.x<SLAVE_ADDRESS_X +204&&tp_dev.x>SLAVE_ADDRESS_X+165)&&(tp_dev.y>SLAVE_ADDRESS_Y-10&&tp_dev.y<SLAVE_ADDRESS_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			cursor_pos = 6;
			LCD_ShowChar(SLAVE_ADDRESS_X+72,  SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ���Ź����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+112, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ��ߵ�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+152, SLAVE_ADDRESS_Y, BLUE, WHITE, '~', 16, 0);							//�ӻ��͵�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+192, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ��ŵ������ʾ
			LCD_ShowChar(SLAVE_QUANTITY_X+72, SLAVE_QUANTITY_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ����������ʾ
			LCD_ShowChar(SCREEN_SLEEP_TIME_X+72, SCREEN_SLEEP_TIME_Y, BLUE, WHITE, ' ', 16, 0);						//��Ļ����ʱ������ʾ
			LCD_ShowChar(QUERY_INTERVAL_TIME_X+72, QUERY_INTERVAL_TIME_Y, BLUE, WHITE, ' ', 16, 0);					//��ѯ�������ʾ
		}	
		//�ж��Ƿ��ڵ�2ҳ����ʾ���λ��(�޸Ĵ��ŵ�)
		if((tp_dev.x<SLAVE_ADDRESS_X +245&&tp_dev.x>SLAVE_ADDRESS_X+205)&&(tp_dev.y>SLAVE_ADDRESS_Y-10&&tp_dev.y<SLAVE_ADDRESS_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			cursor_pos = 7;	
			LCD_ShowChar(SLAVE_ADDRESS_X+72, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);								//�ӻ���Ź����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+112, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ��ߵ�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+152, SLAVE_ADDRESS_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ��͵�ַ�����ʾ
			LCD_ShowChar(SLAVE_ADDRESS_X+192, SLAVE_ADDRESS_Y, BLUE, WHITE, '~', 16, 0);							//�ӻ��ŵ������ʾ
			LCD_ShowChar(SLAVE_QUANTITY_X+72, SLAVE_QUANTITY_Y, BLUE, WHITE, ' ', 16, 0);							//�ӻ����������ʾ
			LCD_ShowChar(SCREEN_SLEEP_TIME_X+72, SCREEN_SLEEP_TIME_Y, BLUE, WHITE, ' ', 16, 0);						//��Ļ����ʱ������ʾ
			LCD_ShowChar(QUERY_INTERVAL_TIME_X+72, QUERY_INTERVAL_TIME_Y, BLUE, WHITE, ' ', 16, 0);					//��ѯ�������ʾ
		}
		//�ж��Ƿ��ڵ�2ҳ�����У׼ʱ��
		if((tp_dev.x<CALIBRATION_TIME_X +90&&tp_dev.x>CALIBRATION_TIME_X)&&(tp_dev.y>CALIBRATION_TIME_Y-10&&tp_dev.y<CALIBRATION_TIME_Y+10)&&PAGE == SHEZHI_PAGE)
		{
			Set_Time();
		}
		//�ж��Ƿ��ڵ�2ҳ����˼Ӻ�
		if((tp_dev.x<PLUS_KEY_X +20&&tp_dev.x>PLUS_KEY_X-20)&&(tp_dev.y>PLUS_KEY_Y-20&&tp_dev.y<PLUS_KEY_Y+20)&&PAGE == SHEZHI_PAGE)
		{
			switch(cursor_pos)
			{
				case 1:
					slave_num++;
					LCD_ShowNum(SLAVE_QUANTITY_X+85, SLAVE_QUANTITY_Y,slave_num, 3, 16);
				break;
				case 2:
					Screen_on_time+=10;
					LCD_ShowNum(SCREEN_SLEEP_TIME_X+85, SCREEN_SLEEP_TIME_Y,Screen_on_time, 3, 16);
				break;
				case 3:
					query_interval++;
					LCD_ShowNum(QUERY_INTERVAL_TIME_X+85, QUERY_INTERVAL_TIME_Y,query_interval, 3, 16);
				break;
				case 4:
					if(flag_num<slave_num-1)
						flag_num++;
					LCD_ShowNum(SLAVE_ADDRESS_X+85,  SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][0], 3, 16);
					LCD_ShowNum(SLAVE_ADDRESS_X+125, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][1], 3, 16);
					LCD_ShowNum(SLAVE_ADDRESS_X+165, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][2], 3, 16);
					LCD_ShowNum(SLAVE_ADDRESS_X+205, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][3], 3, 16);
				break;
				case 5:
					Slave_channel_addr[flag_num][1]++;
					LCD_ShowNum(SLAVE_ADDRESS_X+125, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][1], 3, 16);
				break;
				case 6:
					Slave_channel_addr[flag_num][2]++;
					LCD_ShowNum(SLAVE_ADDRESS_X+165, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][2], 3, 16);
				break;
				case 7:
					Slave_channel_addr[flag_num][3]++;
					LCD_ShowNum(SLAVE_ADDRESS_X+205, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][3], 3, 16);
				break;
			}
		}
		//�ж��Ƿ��ڵ�2ҳ����˼���
		if((tp_dev.x<MINUS_KEY_X +20&&tp_dev.x>MINUS_KEY_X-20)&&(tp_dev.y>MINUS_KEY_Y-20&&tp_dev.y<MINUS_KEY_Y+20)&&PAGE == SHEZHI_PAGE)
		{
			switch(cursor_pos)
			{
				case 1:
					slave_num--;
					LCD_ShowNum(SLAVE_QUANTITY_X+85, SLAVE_QUANTITY_Y,slave_num, 3, 16);
				break;
				case 2:
					Screen_on_time--;
					LCD_ShowNum(SCREEN_SLEEP_TIME_X+85, SCREEN_SLEEP_TIME_Y,Screen_on_time, 3, 16);
				break;
				case 3:
					query_interval--;
					LCD_ShowNum(QUERY_INTERVAL_TIME_X+85, QUERY_INTERVAL_TIME_Y,query_interval, 3, 16);
				break;
				case 4:
					if(flag_num>0)
						flag_num--;
					Slave_channel_addr[flag_num][0] = flag_num;
					LCD_ShowNum(SLAVE_ADDRESS_X+85,  SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][0], 3, 16);
					LCD_ShowNum(SLAVE_ADDRESS_X+125, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][1], 3, 16);
					LCD_ShowNum(SLAVE_ADDRESS_X+165, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][2], 3, 16);
					LCD_ShowNum(SLAVE_ADDRESS_X+205, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][3], 3, 16);
					//Slave_channel_addr[flag_num][0]--;
				break;
				case 5:
					Slave_channel_addr[flag_num][1]--;
					LCD_ShowNum(SLAVE_ADDRESS_X+125, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][1], 3, 16);
				break;
				case 6:
					Slave_channel_addr[flag_num][2]--;
					LCD_ShowNum(SLAVE_ADDRESS_X+165, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][2], 3, 16);
				break;
				case 7:
					Slave_channel_addr[flag_num][3]--;
					LCD_ShowNum(SLAVE_ADDRESS_X+205, SLAVE_ADDRESS_Y,Slave_channel_addr[flag_num][3], 3, 16);
					//AT24CXX_WriteOneByte(AT24CXX_SLAVE_ADDR_ADDR+(i*4)+j, Slave_channel_addr[flag_num][3]);
				break;
			}
		}
		printf("��Ļ������, ����x: %d, y: %d\r\n",tp_dev.x, tp_dev.y);
	}
	EXTI_ClearITPendingBit(EXTI_Line4);  //���EXTI4��·����λ
}




