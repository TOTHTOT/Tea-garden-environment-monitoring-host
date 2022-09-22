#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
#include "word.h"


#define write 0
#define read  1
//IIC总线地址接口定义
#define SCL PAout(4)
#define SDA_OUT PAout(5)
#define SDA_IN PAin(5)
#define IIC_INPUT_MODE_SET()  {GPIOA->CRL&=0xFF0FFFFF;GPIOA->CRL|=0x00800000;}
#define IIC_OUTPUT_MODE_SET() {GPIOA->CRL&=0xFF0FFFFF;GPIOA->CRL|=0x00300000;}
 
void IIC_Init(void)
{
  /*1.开时钟*/
  RCC->APB2ENR |= 1<<2;
  /*2.设置GPIO模式*/
  GPIOA->CRL &= 0xFF00FFFF;
  GPIOA->CRL |= 0x00330000;
  /*3.设置GPIO空闲电平*/
  GPIOA->ODR |= 0x3<<4;	
  //printf("IIC_Init OK！\n");
}	
 
/*******************************************************************
功能:启动I2C总线,即发送I2C起始条件. SCL为高电平期间 SDA出现下降沿
********************************************************************/
void IIC_Start(void)
{
  IIC_OUTPUT_MODE_SET();
  SDA_OUT=1;
  SCL=1;
  SDA_OUT=0;
	SCL=0;
}
 
/*******************************************************************
功能:结束I2C总线,即发送I2C结束条件. SCL为高电平期间 SDA出现上升沿 
********************************************************************/
void IIC_Stop(void)
{
	IIC_OUTPUT_MODE_SET();
	SDA_OUT=0;  	
	SCL=1;
	SDA_OUT=1; 
}
 
/*主机发送ACK*/
void IIC_ACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0; 
  SDA_OUT=0;   
  SCL=1;                
  SCL=0;                        
}
 
/*主机不发送ACK*/
void IIC_NACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0;
  SDA_OUT=1;     
  SCL=1;                  
  SCL=0;                        
}
 
/*主机等待从机的ACK*/
u8 IIC_wait_ACK(void)
{
	u8 t = 200;
	IIC_OUTPUT_MODE_SET();
	SDA_OUT=1;//8位发送完后释放数据线，准备接收应答位 
	SCL=0;
	IIC_INPUT_MODE_SET();
	while(SDA_IN)//等待应答
	{
		t--;
		delay_us(1); 
		if(t==0)
		{
			SCL=0;
			return 1;
		}
	}     
	SCL=1;
	SCL=0;                
	return 0;	
}
 
/*******************************************************************
字节数据发送函数               
函数原型: void  SendByte(UCHAR c);
功能:将数据c发送出去,可以是地址,也可以是数据
********************************************************************/
void  IIC_SendByte(u8 byte)
{
	u8  BitCnt;
	IIC_OUTPUT_MODE_SET();
	SCL=0;
	for(BitCnt=0;BitCnt<8;BitCnt++)//要传送的数据长度为8位
	{
		if(byte&0x80) SDA_OUT=1;//判断发送位
		else SDA_OUT=0; 
		byte<<=1;
		SCL=1;
		SCL=0;
	}
}
/*******************************************************************
 字节数据接收函数               
函数原型: UCHAR  RcvByte();
功能: 用来接收从器件传来的数据  
********************************************************************/    
u8 IIC_RcvByte(void)
{
  u8 retc;
  u8 BitCnt;
  retc=0; 
  IIC_INPUT_MODE_SET();//置数据线为输入方式                   
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {  
		SCL=0;	//置时钟线为低，准备接收数据位
		delay_us(1);               
		SCL=1;	//置时钟线为高使数据线上数据有效                
		retc=retc<<1;
		if(SDA_IN) retc |=1;//读数据位,接收的数据位放入retc中 
  }
  SCL=0;    
  return(retc);

}



u8 OLED_GRAM[8][128];/*定义OLED显存数组*/
 
/*
函数功能: 向SSD1106写入一个字节
函数参数: dat:要写入的数据/命令
          cmd:数据/命令标志  0表示命令  1表示数据
*/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	IIC_Start();								//开始传输
	IIC_SendByte(0x78);					//传输地址
	IIC_wait_ACK();							//接收1306返回的确认信号
	if(cmd) IIC_SendByte(0x40);	//发送控制字节，写数据
	else IIC_SendByte(0x00);		//发送控制字节，写命令	
	IIC_wait_ACK();							//接收1306返回的确认信号
	IIC_SendByte(dat);					//发送数据字节
	IIC_wait_ACK();							//接收1306返回的确认信号
	IIC_Stop();									//结束传输
}
 
/*********************OLED初始化************************************/
void OLED_Init(void)
{
	IIC_Init();
	delay_ms(500);								//初始化之前的延时很重要！
	OLED_WR_Byte(0xAE,OLED_CMD); //关闭显示
	OLED_WR_Byte(0x00,OLED_CMD);//设置低列地址
	OLED_WR_Byte(0x10,OLED_CMD);//设置高列地址
	OLED_WR_Byte(0x40,OLED_CMD);//设置起始行地址,集映射RAM显示起始行(0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//设置对比度控制寄存器
	OLED_WR_Byte(0xCF,OLED_CMD);//设置SEG输出电流亮度
	OLED_WR_Byte(0xA1,OLED_CMD);//段重定义设置,bit0:0,0->0;1,0->127; 0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//设置正常显示(设置显示方式;bit0:1,反相显示;0,正常显示	)
	OLED_WR_Byte(0xA8,OLED_CMD);//设置驱动路数 设置多路复用比(1比64)
	OLED_WR_Byte(0x3F,OLED_CMD);//1/64 duty(默认0X3F(1/64))
	OLED_WR_Byte(0xD3,OLED_CMD);//设置显示偏移位移映射RAM计数器(0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xD5,OLED_CMD);//设置显示时钟分频比/振荡器频率
	OLED_WR_Byte(0x80,OLED_CMD);//设置分频比，设置时钟为100帧/秒
	OLED_WR_Byte(0xD9,OLED_CMD);//设置预充电周期
	OLED_WR_Byte(0xF1,OLED_CMD);//设置预充15个时钟，放电1个时钟([3:0],PHASE 1;[7:4],PHASE 2;)
	OLED_WR_Byte(0xDA,OLED_CMD);//设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD);//[5:4]配置
	OLED_WR_Byte(0xDB,OLED_CMD);//设置VCOMH 电压倍率
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM 释放电压([6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;)
	OLED_WR_Byte(0x20,OLED_CMD);//设置页面寻址模式(0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
	OLED_WR_Byte(0x8D,OLED_CMD);//设置充电泵启用/禁用
	OLED_WR_Byte(0x14,OLED_CMD);//设置(0x10禁用,0x14启用)
	OLED_WR_Byte(0xA4,OLED_CMD);// 全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏) (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// 设置显示方式;bit0:1,反相显示;0,正常显示 (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//开启显示	
	OLED_Clear();
	OLED_SetCursorAddrese(0,0);
	//printf("OLED Init OK !\n");
}
/*函数功能:开启OLED显示 */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令（设置电荷泵）
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON （开启电荷泵）
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON（OLED唤醒）
}
/*函数功能:关闭OLED显示*/ 
 
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令 （设置电荷泵）
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF （关闭电荷泵）
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF （OLED休眠）
}
 
 
/*
函数功能:清屏函数,清完屏,整个屏幕是黑色的!  
说明: 清屏就是向OLED里写0
      对于OLED屏,0就不显示,1才会在屏幕上显示出来一个点
*/
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xB0+i,OLED_CMD);//设置页地址（0~7）
		OLED_WR_Byte(0x02,OLED_CMD);  //设置显示位置―列低地址
		OLED_WR_Byte(0x10,OLED_CMD);  //设置显示位置―列高地址 
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(0x00,OLED_DATA);
		}
	}
}
/*
函数功能：在显存数组上画一个点
函数参数：x，y为点的横纵坐标   c为这个点的亮灭（1亮0灭）
参数范围：x 0~128  y 0~8 
每一个数据是 低位在前，高位在后
*/
void OLED_Draw_Point(u8 x,u8 y,u8 c)
{
	u8 page,addr;
	page = y/8; //显存的页地址
	addr = y%8; //显存的一个字节数据中c所在的位置 
	if(c) OLED_GRAM[page][x] |= 1<<addr;
	else  OLED_GRAM[page][x] &= ~(1<<addr);
}
 
/*
函数功能: 设置光标位置
函数参数: x列的起始位置(0~127)
				  y页的起始位置(0~7)
比如: 0x8  高4位0000   低4位1000 
*/
void OLED_SetCursorAddrese(u8 x,u8 y)
{
		OLED_WR_Byte(0xB0+y,OLED_CMD); 					//设置页地址
	  //第6列显示数据  6被分成2个4位(高位和低位)。
		OLED_WR_Byte((x&0xF0)>>4|0x10,OLED_CMD);//设置列高起始地址(半字节)
		OLED_WR_Byte((x&0x0F)|0x02,OLED_CMD);   //设置列低起始地址(半字节)			
}
/*****************************************************
** 函数名称：OLED_VerticalDisplay
** 函数功能：实现OLED垂直滚动范围配置
** 参    数：1.toprow：设置滚动起始行
**           2.scrollrow：设置滚动行数
**           注意：toprow+scrollrow<64
** 函数返回：无
******************************************************/
void OLED_VerticalDisplay(u8 toprow,u8 scrollrow)
{
	OLED_WR_Byte(SET_VERTICAL_SCROLL_AREA,OLED_CMD);
	OLED_WR_Byte(toprow,OLED_CMD);
	OLED_WR_Byte(scrollrow,OLED_CMD);
}
 
/*****************************************************
** 函数名称：OledScrollStop
** 函数功能：关闭滚屏功能
******************************************************/ 
void OledScrollStop(void)
{
	OLED_WR_Byte(DEACTIVATE_SCROLL,OLED_CMD);
}
 
/*
函数功能: 按页清屏 （0~7）
*/
void OLED_PageClear(u8 page)
{
		u8 j;
		OLED_WR_Byte(0xB0+page,OLED_CMD); //设置页地址
		OLED_WR_Byte(0x10,OLED_CMD);      //设置列高起始地址(半字节)
		OLED_WR_Byte(0x02,OLED_CMD);      //设置列低起始地址(半字节)
		for(j=0;j<128;j++)
		{
				OLED_WR_Byte(0,OLED_DATA); //写数据
		}
}
 
/*
函数功能：更新显存到OLED屏幕上*/
void OLED_Refresh_Screen(void)
{
	u8 page,list;  //定义页地址和列地址
	for(page=0;page<8;page++)
	{
		//1.3寸oled要从0x02开始
		OLED_WR_Byte(0xB0+page,OLED_CMD); //设置页地址（0~7）
		OLED_WR_Byte(0x02,OLED_CMD);      //设置显示位置―列低地址
		OLED_WR_Byte(0x10,OLED_CMD);      //设置显示位置―列高地址 
		for(list=0;list<128;list++)
		{
			OLED_WR_Byte(OLED_GRAM[page][list],OLED_DATA);
		}
	}
	memset(OLED_GRAM,0,sizeof(OLED_GRAM));	/*清空显存数组*/
}
 
u8 Oled_Display(u8 x ,u8 y ,u8 width , u8 height)
{
	u8 i,j;
	if(width>128) width=128;
	if(height>64) height=64;
	/*1. OLED显示屏初始化*/
	OLED_Clear();	//清屏
	for(i=0;i<height/8;i++)
	{
		OLED_WR_Byte(0xB0+i,OLED_CMD); //设置页地址（0~7）
		OLED_WR_Byte(0x02,OLED_CMD);      //设置显示位置―列低地址
		OLED_WR_Byte(0x10,OLED_CMD);      //设置显示位置―列高地址 
		for(j=0;j<width;j++)
		{ 
//			OLED_WR_Byte(BMP[i][j],OLED_DATA);
		}
	}
	delay_ms(10);
  return 0;
}
 

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{      	
	unsigned char c=0,i=0;	
	c=chr-' ';//得到偏移后的值			
	if(x>128-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_SetCursorAddrese(x,y); //设置光标的位置
		for(i=0;i<8;i++)
		OLED_WR_Byte(ASCII_8_16[c][i],OLED_DATA);
		OLED_SetCursorAddrese(x,y+1); //设置光标的位置
		for(i=0;i<8;i++)
		OLED_WR_Byte(ASCII_8_16[c][i+8],OLED_DATA);
	}
	else 
	{	
		OLED_SetCursorAddrese(x,y); //设置光标的位置
		for(i=0;i<6;i++)
		OLED_WR_Byte(ASCII_6_8[c][i],OLED_DATA);
	}
	
}

//显示一个字符号串
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}

void OLED_DisplayString(u8 x,u8 y,u8 width,u8 height,u8 *str)
{
	u8 addr=0,i;
	u16 font=0;
	while(*str!='\0') //连续显示
	{
		//取模从空格开始的，计算下标
		//写8*16ASCII字符的上半部分
		if(*str >= ' '&& *str <= '~') //显示英文
		{
			addr=*str-' '; //取模从空格开始的，计算下标
			//写8*16ASCII字符的上半部分
			OLED_SetCursorAddrese(x,y); //设置光标的位置
			for(i=0;i<width/2;i++)      //横向写width列
			{
				 OLED_WR_Byte(ASCII_8_16[addr][i],OLED_DATA); 
			}
			//写8*16ASCII字符的下半部分
			OLED_SetCursorAddrese(x,y+1); //设置光标的位置
			for(i=0;i<width/2;i++)        //横向写width列
			{
				 OLED_WR_Byte(ASCII_8_16[addr][i+width/2],OLED_DATA); 
			}
			str++;//继续显示下一个字符
			x+=width/2; //在下一个位置继续显示数据			
		}
		else //显示中文
		{
			OLED_SetCursorAddrese(x,y); //设置光标的位置
			font=((*str)<<8)+(*(str+1));
			switch(font)
			{
				//按照汉字内码查询
				case 0xCEC2:addr=0;break;//温
				case 0xCAAA:addr=1;break;//湿
				case 0xB6C8:addr=2;break;//度
				case 0xCAFD:addr=3;break;//数
				case 0xBEDD:addr=4;break;//据
				case 0xB3C9:addr=5;break;//成
				case 0xB9A6:addr=6;break;//功
				case 0xB7A2:addr=7;break;//发
				case 0xCBCD:addr=8;break;//送
				case 0xC1AC:addr=9;break;//连
				case 0xBDD3:addr=10;break;//接
				case 0xB7FE:addr=11;break;//服
				case 0xCEF1:addr=12;break;//务
				case 0xC6F7:addr=13;break;//器
				case 0xC9CF:addr=14;break;//上
				case 0xCFC2:addr=15;break;//下
				case 0xCFDE:addr=16;break;//限
				case 0xD6B5:addr=17;break;//值
				case 0xC9E8:addr=18;break;//设
				case 0xD6C3:addr=19;break;//置
				case 0xB4AB:addr=20;break;//传
				case 0xD6DC:addr=21;break;//周
				case 0xC6DA:addr=22;break;//期
				case 0xBFD8:addr=23;break;//控
				case 0xD6C6:addr=24;break;//制
				case 0xB2CE:addr=25;break;//参
				case 0xD7DC:addr=26;break;//总
				case 0xB1ED:addr=27;break;//表
				case 0xB1B8:addr=28;break;//备
				case 0xCDF8:addr=29;break;//网
				case 0xC2E7:addr=30;break;//络
				case 0xD0C5:addr=31;break;//信
				case 0xCFA2:addr=32;break;//息
				case 0xCAA7:addr=33;break;//失
				case 0xB0DC:addr=34;break;//败
				case 0xB1A3:addr=35;break;//保
				case 0xB4E6:addr=36;break;//存
				case 0xB9FD:addr=37;break;//过
				case 0xB8DF:addr=38;break;//高
				case 0xA1E6:addr=39;break;//℃
				case 0xD0C7:addr=40;break;//星
				case 0xC8D5:addr=42;break;//日
				case 0xD2BB:addr=43;break;//一
				case 0xB6FE:addr=44;break;//二
				case 0xC8FD:addr=45;break;//三
				case 0xCBC4:addr=46;break;//四
				case 0xCEE5:addr=47;break;//五
				case 0xC1F9:addr=48;break;//六
				case 0xC4EA:addr=49;break;//年
				case 0xD4C2:addr=50;break;//月
				case 0xCED2:addr=52;break;//我
				case 0xB0AE:addr=53;break;//爱
				case 0xC8C4:addr=54;break;//饶
				case 0xBFAD:addr=55;break;//凯
				case 0xD1E0:addr=56;break;//燕
				default : break;
			}
			for(i=0;i<width;i++) //横向写width列
			{
				 OLED_WR_Byte(ChineseFont_16_16[addr][i],OLED_DATA); 
			}
			
			//写8*16ASCII字符的下半部分
			OLED_SetCursorAddrese(x,y+1); //设置光标的位置
			for(i=0;i<width;i++)          //横向写width列
			{
				 OLED_WR_Byte(ChineseFont_16_16[addr][i+width],OLED_DATA); 
			}
			str+=2;//继续显示下一个字符
			x+=width; //在下一个位置继续显示数据
		}
	}
}
void OLED_Menu_Display()
{
//	OLED_ShowString(0,0,"#123",16);
//	OLED_DisplayString(0,0,16,16,"1234567890");
//	OLED_DisplayString(0,2,16,16,"我爱饶凯燕");
	
//	OLED_ShowString(0,2,"T :",8);
//	OLED_ShowString(0,4,"RH:",8);
//	OLED_ShowString(0,6,"L :",8);
//	OLED_ShowString(60,2,"SH  :",8);
//	OLED_ShowString(60,4,"CO2 :",8);
//	OLED_ShowString(60,6,"TVOC:",8);
	
//	OLED_DisplayString(0,4,16,16,"RH:");
//	OLED_DisplayString(0,6,16,16,"L :");
//	OLED_DisplayString(60,2,16,16,"SH:");
//	OLED_DisplayString(60,4,16,16,"CO2:");
//	OLED_DisplayString(60,6,16,16,"TVOC");
}









