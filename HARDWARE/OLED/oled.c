#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"
#include "word.h"


#define write 0
#define read  1
//IIC���ߵ�ַ�ӿڶ���
#define SCL PAout(4)
#define SDA_OUT PAout(5)
#define SDA_IN PAin(5)
#define IIC_INPUT_MODE_SET()  {GPIOA->CRL&=0xFF0FFFFF;GPIOA->CRL|=0x00800000;}
#define IIC_OUTPUT_MODE_SET() {GPIOA->CRL&=0xFF0FFFFF;GPIOA->CRL|=0x00300000;}
 
void IIC_Init(void)
{
  /*1.��ʱ��*/
  RCC->APB2ENR |= 1<<2;
  /*2.����GPIOģʽ*/
  GPIOA->CRL &= 0xFF00FFFF;
  GPIOA->CRL |= 0x00330000;
  /*3.����GPIO���е�ƽ*/
  GPIOA->ODR |= 0x3<<4;	
  //printf("IIC_Init OK��\n");
}	
 
/*******************************************************************
����:����I2C����,������I2C��ʼ����. SCLΪ�ߵ�ƽ�ڼ� SDA�����½���
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
����:����I2C����,������I2C��������. SCLΪ�ߵ�ƽ�ڼ� SDA���������� 
********************************************************************/
void IIC_Stop(void)
{
	IIC_OUTPUT_MODE_SET();
	SDA_OUT=0;  	
	SCL=1;
	SDA_OUT=1; 
}
 
/*��������ACK*/
void IIC_ACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0; 
  SDA_OUT=0;   
  SCL=1;                
  SCL=0;                        
}
 
/*����������ACK*/
void IIC_NACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0;
  SDA_OUT=1;     
  SCL=1;                  
  SCL=0;                        
}
 
/*�����ȴ��ӻ���ACK*/
u8 IIC_wait_ACK(void)
{
	u8 t = 200;
	IIC_OUTPUT_MODE_SET();
	SDA_OUT=1;//8λ��������ͷ������ߣ�׼������Ӧ��λ 
	SCL=0;
	IIC_INPUT_MODE_SET();
	while(SDA_IN)//�ȴ�Ӧ��
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
�ֽ����ݷ��ͺ���               
����ԭ��: void  SendByte(UCHAR c);
����:������c���ͳ�ȥ,�����ǵ�ַ,Ҳ����������
********************************************************************/
void  IIC_SendByte(u8 byte)
{
	u8  BitCnt;
	IIC_OUTPUT_MODE_SET();
	SCL=0;
	for(BitCnt=0;BitCnt<8;BitCnt++)//Ҫ���͵����ݳ���Ϊ8λ
	{
		if(byte&0x80) SDA_OUT=1;//�жϷ���λ
		else SDA_OUT=0; 
		byte<<=1;
		SCL=1;
		SCL=0;
	}
}
/*******************************************************************
 �ֽ����ݽ��պ���               
����ԭ��: UCHAR  RcvByte();
����: �������մ���������������  
********************************************************************/    
u8 IIC_RcvByte(void)
{
  u8 retc;
  u8 BitCnt;
  retc=0; 
  IIC_INPUT_MODE_SET();//��������Ϊ���뷽ʽ                   
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {  
		SCL=0;	//��ʱ����Ϊ�ͣ�׼����������λ
		delay_us(1);               
		SCL=1;	//��ʱ����Ϊ��ʹ��������������Ч                
		retc=retc<<1;
		if(SDA_IN) retc |=1;//������λ,���յ�����λ����retc�� 
  }
  SCL=0;    
  return(retc);

}



u8 OLED_GRAM[8][128];/*����OLED�Դ�����*/
 
/*
��������: ��SSD1106д��һ���ֽ�
��������: dat:Ҫд�������/����
          cmd:����/�����־  0��ʾ����  1��ʾ����
*/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	IIC_Start();								//��ʼ����
	IIC_SendByte(0x78);					//�����ַ
	IIC_wait_ACK();							//����1306���ص�ȷ���ź�
	if(cmd) IIC_SendByte(0x40);	//���Ϳ����ֽڣ�д����
	else IIC_SendByte(0x00);		//���Ϳ����ֽڣ�д����	
	IIC_wait_ACK();							//����1306���ص�ȷ���ź�
	IIC_SendByte(dat);					//���������ֽ�
	IIC_wait_ACK();							//����1306���ص�ȷ���ź�
	IIC_Stop();									//��������
}
 
/*********************OLED��ʼ��************************************/
void OLED_Init(void)
{
	IIC_Init();
	delay_ms(500);								//��ʼ��֮ǰ����ʱ����Ҫ��
	OLED_WR_Byte(0xAE,OLED_CMD); //�ر���ʾ
	OLED_WR_Byte(0x00,OLED_CMD);//���õ��е�ַ
	OLED_WR_Byte(0x10,OLED_CMD);//���ø��е�ַ
	OLED_WR_Byte(0x40,OLED_CMD);//������ʼ�е�ַ,��ӳ��RAM��ʾ��ʼ��(0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//���öԱȶȿ��ƼĴ���
	OLED_WR_Byte(0xCF,OLED_CMD);//����SEG�����������
	OLED_WR_Byte(0xA1,OLED_CMD);//���ض�������,bit0:0,0->0;1,0->127; 0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8,OLED_CMD);//����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��   0xc0���·��� 0xc8����
	OLED_WR_Byte(0xA6,OLED_CMD);//����������ʾ(������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	)
	OLED_WR_Byte(0xA8,OLED_CMD);//��������·�� ���ö�·���ñ�(1��64)
	OLED_WR_Byte(0x3F,OLED_CMD);//1/64 duty(Ĭ��0X3F(1/64))
	OLED_WR_Byte(0xD3,OLED_CMD);//������ʾƫ��λ��ӳ��RAM������(0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xD5,OLED_CMD);//������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WR_Byte(0x80,OLED_CMD);//���÷�Ƶ�ȣ�����ʱ��Ϊ100֡/��
	OLED_WR_Byte(0xD9,OLED_CMD);//����Ԥ�������
	OLED_WR_Byte(0xF1,OLED_CMD);//����Ԥ��15��ʱ�ӣ��ŵ�1��ʱ��([3:0],PHASE 1;[7:4],PHASE 2;)
	OLED_WR_Byte(0xDA,OLED_CMD);//����COMӲ����������
	OLED_WR_Byte(0x12,OLED_CMD);//[5:4]����
	OLED_WR_Byte(0xDB,OLED_CMD);//����VCOMH ��ѹ����
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM �ͷŵ�ѹ([6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;)
	OLED_WR_Byte(0x20,OLED_CMD);//����ҳ��Ѱַģʽ(0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	OLED_WR_Byte(0x8D,OLED_CMD);//���ó�������/����
	OLED_WR_Byte(0x14,OLED_CMD);//����(0x10����,0x14����)
	OLED_WR_Byte(0xA4,OLED_CMD);// ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����) (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// ������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);//������ʾ	
	OLED_Clear();
	OLED_SetCursorAddrese(0,0);
	//printf("OLED Init OK !\n");
}
/*��������:����OLED��ʾ */
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC������õ�ɱã�
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON ��������ɱã�
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON��OLED���ѣ�
}
/*��������:�ر�OLED��ʾ*/ 
 
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC���� �����õ�ɱã�
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF ���رյ�ɱã�
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF ��OLED���ߣ�
}
 
 
/*
��������:��������,������,������Ļ�Ǻ�ɫ��!  
˵��: ����������OLED��д0
      ����OLED��,0�Ͳ���ʾ,1�Ż�����Ļ����ʾ����һ����
*/
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xB0+i,OLED_CMD);//����ҳ��ַ��0~7��
		OLED_WR_Byte(0x02,OLED_CMD);  //������ʾλ�èD�е͵�ַ
		OLED_WR_Byte(0x10,OLED_CMD);  //������ʾλ�èD�иߵ�ַ 
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(0x00,OLED_DATA);
		}
	}
}
/*
�������ܣ����Դ������ϻ�һ����
����������x��yΪ��ĺ�������   cΪ����������1��0��
������Χ��x 0~128  y 0~8 
ÿһ�������� ��λ��ǰ����λ�ں�
*/
void OLED_Draw_Point(u8 x,u8 y,u8 c)
{
	u8 page,addr;
	page = y/8; //�Դ��ҳ��ַ
	addr = y%8; //�Դ��һ���ֽ�������c���ڵ�λ�� 
	if(c) OLED_GRAM[page][x] |= 1<<addr;
	else  OLED_GRAM[page][x] &= ~(1<<addr);
}
 
/*
��������: ���ù��λ��
��������: x�е���ʼλ��(0~127)
				  yҳ����ʼλ��(0~7)
����: 0x8  ��4λ0000   ��4λ1000 
*/
void OLED_SetCursorAddrese(u8 x,u8 y)
{
		OLED_WR_Byte(0xB0+y,OLED_CMD); 					//����ҳ��ַ
	  //��6����ʾ����  6���ֳ�2��4λ(��λ�͵�λ)��
		OLED_WR_Byte((x&0xF0)>>4|0x10,OLED_CMD);//�����и���ʼ��ַ(���ֽ�)
		OLED_WR_Byte((x&0x0F)|0x02,OLED_CMD);   //�����е���ʼ��ַ(���ֽ�)			
}
/*****************************************************
** �������ƣ�OLED_VerticalDisplay
** �������ܣ�ʵ��OLED��ֱ������Χ����
** ��    ����1.toprow�����ù�����ʼ��
**           2.scrollrow�����ù�������
**           ע�⣺toprow+scrollrow<64
** �������أ���
******************************************************/
void OLED_VerticalDisplay(u8 toprow,u8 scrollrow)
{
	OLED_WR_Byte(SET_VERTICAL_SCROLL_AREA,OLED_CMD);
	OLED_WR_Byte(toprow,OLED_CMD);
	OLED_WR_Byte(scrollrow,OLED_CMD);
}
 
/*****************************************************
** �������ƣ�OledScrollStop
** �������ܣ��رչ�������
******************************************************/ 
void OledScrollStop(void)
{
	OLED_WR_Byte(DEACTIVATE_SCROLL,OLED_CMD);
}
 
/*
��������: ��ҳ���� ��0~7��
*/
void OLED_PageClear(u8 page)
{
		u8 j;
		OLED_WR_Byte(0xB0+page,OLED_CMD); //����ҳ��ַ
		OLED_WR_Byte(0x10,OLED_CMD);      //�����и���ʼ��ַ(���ֽ�)
		OLED_WR_Byte(0x02,OLED_CMD);      //�����е���ʼ��ַ(���ֽ�)
		for(j=0;j<128;j++)
		{
				OLED_WR_Byte(0,OLED_DATA); //д����
		}
}
 
/*
�������ܣ������Դ浽OLED��Ļ��*/
void OLED_Refresh_Screen(void)
{
	u8 page,list;  //����ҳ��ַ���е�ַ
	for(page=0;page<8;page++)
	{
		//1.3��oledҪ��0x02��ʼ
		OLED_WR_Byte(0xB0+page,OLED_CMD); //����ҳ��ַ��0~7��
		OLED_WR_Byte(0x02,OLED_CMD);      //������ʾλ�èD�е͵�ַ
		OLED_WR_Byte(0x10,OLED_CMD);      //������ʾλ�èD�иߵ�ַ 
		for(list=0;list<128;list++)
		{
			OLED_WR_Byte(OLED_GRAM[page][list],OLED_DATA);
		}
	}
	memset(OLED_GRAM,0,sizeof(OLED_GRAM));	/*����Դ�����*/
}
 
u8 Oled_Display(u8 x ,u8 y ,u8 width , u8 height)
{
	u8 i,j;
	if(width>128) width=128;
	if(height>64) height=64;
	/*1. OLED��ʾ����ʼ��*/
	OLED_Clear();	//����
	for(i=0;i<height/8;i++)
	{
		OLED_WR_Byte(0xB0+i,OLED_CMD); //����ҳ��ַ��0~7��
		OLED_WR_Byte(0x02,OLED_CMD);      //������ʾλ�èD�е͵�ַ
		OLED_WR_Byte(0x10,OLED_CMD);      //������ʾλ�èD�иߵ�ַ 
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
	c=chr-' ';//�õ�ƫ�ƺ��ֵ			
	if(x>128-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_SetCursorAddrese(x,y); //���ù���λ��
		for(i=0;i<8;i++)
		OLED_WR_Byte(ASCII_8_16[c][i],OLED_DATA);
		OLED_SetCursorAddrese(x,y+1); //���ù���λ��
		for(i=0;i<8;i++)
		OLED_WR_Byte(ASCII_8_16[c][i+8],OLED_DATA);
	}
	else 
	{	
		OLED_SetCursorAddrese(x,y); //���ù���λ��
		for(i=0;i<6;i++)
		OLED_WR_Byte(ASCII_6_8[c][i],OLED_DATA);
	}
	
}

//��ʾһ���ַ��Ŵ�
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
	while(*str!='\0') //������ʾ
	{
		//ȡģ�ӿո�ʼ�ģ������±�
		//д8*16ASCII�ַ����ϰ벿��
		if(*str >= ' '&& *str <= '~') //��ʾӢ��
		{
			addr=*str-' '; //ȡģ�ӿո�ʼ�ģ������±�
			//д8*16ASCII�ַ����ϰ벿��
			OLED_SetCursorAddrese(x,y); //���ù���λ��
			for(i=0;i<width/2;i++)      //����дwidth��
			{
				 OLED_WR_Byte(ASCII_8_16[addr][i],OLED_DATA); 
			}
			//д8*16ASCII�ַ����°벿��
			OLED_SetCursorAddrese(x,y+1); //���ù���λ��
			for(i=0;i<width/2;i++)        //����дwidth��
			{
				 OLED_WR_Byte(ASCII_8_16[addr][i+width/2],OLED_DATA); 
			}
			str++;//������ʾ��һ���ַ�
			x+=width/2; //����һ��λ�ü�����ʾ����			
		}
		else //��ʾ����
		{
			OLED_SetCursorAddrese(x,y); //���ù���λ��
			font=((*str)<<8)+(*(str+1));
			switch(font)
			{
				//���պ��������ѯ
				case 0xCEC2:addr=0;break;//��
				case 0xCAAA:addr=1;break;//ʪ
				case 0xB6C8:addr=2;break;//��
				case 0xCAFD:addr=3;break;//��
				case 0xBEDD:addr=4;break;//��
				case 0xB3C9:addr=5;break;//��
				case 0xB9A6:addr=6;break;//��
				case 0xB7A2:addr=7;break;//��
				case 0xCBCD:addr=8;break;//��
				case 0xC1AC:addr=9;break;//��
				case 0xBDD3:addr=10;break;//��
				case 0xB7FE:addr=11;break;//��
				case 0xCEF1:addr=12;break;//��
				case 0xC6F7:addr=13;break;//��
				case 0xC9CF:addr=14;break;//��
				case 0xCFC2:addr=15;break;//��
				case 0xCFDE:addr=16;break;//��
				case 0xD6B5:addr=17;break;//ֵ
				case 0xC9E8:addr=18;break;//��
				case 0xD6C3:addr=19;break;//��
				case 0xB4AB:addr=20;break;//��
				case 0xD6DC:addr=21;break;//��
				case 0xC6DA:addr=22;break;//��
				case 0xBFD8:addr=23;break;//��
				case 0xD6C6:addr=24;break;//��
				case 0xB2CE:addr=25;break;//��
				case 0xD7DC:addr=26;break;//��
				case 0xB1ED:addr=27;break;//��
				case 0xB1B8:addr=28;break;//��
				case 0xCDF8:addr=29;break;//��
				case 0xC2E7:addr=30;break;//��
				case 0xD0C5:addr=31;break;//��
				case 0xCFA2:addr=32;break;//Ϣ
				case 0xCAA7:addr=33;break;//ʧ
				case 0xB0DC:addr=34;break;//��
				case 0xB1A3:addr=35;break;//��
				case 0xB4E6:addr=36;break;//��
				case 0xB9FD:addr=37;break;//��
				case 0xB8DF:addr=38;break;//��
				case 0xA1E6:addr=39;break;//��
				case 0xD0C7:addr=40;break;//��
				case 0xC8D5:addr=42;break;//��
				case 0xD2BB:addr=43;break;//һ
				case 0xB6FE:addr=44;break;//��
				case 0xC8FD:addr=45;break;//��
				case 0xCBC4:addr=46;break;//��
				case 0xCEE5:addr=47;break;//��
				case 0xC1F9:addr=48;break;//��
				case 0xC4EA:addr=49;break;//��
				case 0xD4C2:addr=50;break;//��
				case 0xCED2:addr=52;break;//��
				case 0xB0AE:addr=53;break;//��
				case 0xC8C4:addr=54;break;//��
				case 0xBFAD:addr=55;break;//��
				case 0xD1E0:addr=56;break;//��
				default : break;
			}
			for(i=0;i<width;i++) //����дwidth��
			{
				 OLED_WR_Byte(ChineseFont_16_16[addr][i],OLED_DATA); 
			}
			
			//д8*16ASCII�ַ����°벿��
			OLED_SetCursorAddrese(x,y+1); //���ù���λ��
			for(i=0;i<width;i++)          //����дwidth��
			{
				 OLED_WR_Byte(ChineseFont_16_16[addr][i+width],OLED_DATA); 
			}
			str+=2;//������ʾ��һ���ַ�
			x+=width; //����һ��λ�ü�����ʾ����
		}
	}
}
void OLED_Menu_Display()
{
//	OLED_ShowString(0,0,"#123",16);
//	OLED_DisplayString(0,0,16,16,"1234567890");
//	OLED_DisplayString(0,2,16,16,"�Ұ��Ŀ���");
	
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









