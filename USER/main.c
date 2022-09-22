#include "sys.h"
#include "main.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "usart2.h"
#include "usart3.h"
#include "lora.h"
#include "string.h"	 
#include "timer.h"
#include "stdio.h"
#include "exti.h"
#include "rtc.h"
#include "iwdg.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"
#include "test.h"
#include "24cxx.h"
#include "myiic.h"
#include "key.h"
#include "spi.h"
#include "mmc_sd.h"	
#include "malloc.h"
#include "ff.h"  
#include "exfuns.h"
#include "fattester.h"	 
//#include "piclib.h"	

u8 slave_num = 1;												//�ӻ�����,���������������,Ĭ��Ϊ1
u8 query_interval = 2;											//��ѯ���ʱ����СΪ1(500ms), �������޸�
u8 Screen_on_time = 60;											//��Ļ����ʱ�䵹��ʱ, �������޸�
u8 Slave_channel_addr[10][4];									//10���ӻ��ĵ�ַ���ŵ�
u8 slave = 1, mf_open_sta;										//���ĸ��ӻ�������Ϣ
u8 retry_to_lora = 0;
u8 i, pos, head, receive_data[100], *str, now_time_sec = 0;
u8 write_file_dat[100], dat[20];
u16 light_value, soil_humidity,  eco2 = 0, tvoc;
u16 colorTemp=0, j;
u32 sd_total, sd_free, file_size;
float soil_humidity_value, eco2_value, tvoc_value;
u8 str_temp[20];
u8 receive_complete = 0;
u8 * read_history_data;
u8 en_save_data = 0;
extern _TDateTime TDATATIME;		//ʱ��ṹ��
int int_history_num = 0;
u8 valid_data[3];
u8 PAGE = 0;
static struct Sensor_Data
{
	u8 t;
	u8 rh;
	u16 light;
	float sh;
	float eco2;
	float tvoc;
}Slave_Avr[7];

int main(void)
{		

	SystemInit();												 //ϵͳ��ʼ��ʱ���� 
	delay_init();	    										 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		  	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 									   	//���ڳ�ʼ��Ϊ115200
	LED_Init();
	USART2_Init(115200);										//��esp8266ͨ��
	usart3_init(115200);			
	LORA_Init();
	AT24CXX_Init();												//��ʼ��at24c02
//	EXTIX_Init();
	exfuns_init();												//Ϊfatfs��ر��������ڴ�
//	IWDG_Init(6,4000); 											//���Ƶ��Ϊ 256,����ֵΪ 625,���ʱ��Ϊ 25.6s
	LCD_Init();	   												//Һ������ʼ��
	TP_Init();
	KEY_Init();
	mem_init();													//��ʼ���ڴ��	
	while(SD_Initialize())										//��ⲻ��SD��
	{
		
		Show_Str(50,25,BLUE,WHITE,"SD Card Erro!!!",16,0);
		delay_ms(500);					
		Show_Str(50,25,BLUE,WHITE,"Please Check!!!",16,0);
		delay_ms(500);
		LED0=!LED0;//DS0��˸
	}

	Show_Str(50,50,BLUE,WHITE,"SD Card OK!!!",16,0);
	exfuns_init();												//Ϊfatfs��ر��������ڴ�				 
	f_mount(fs,"0:",1); 										//����SD�� 
	Rotate_Test();												//��ʼ������
	Set_Time();													//��ʼ��ʱ��
	Get_config_Data();											//��at24c02��ȡ������Ϣ
	while(AUX_Check())											//lora��æ
	{
		printf("LORA��æ\r\n");
		delay_ms(500);
	}
	printf("LORA����\r\n");
	Get_History_Num();											//ÿ�ο�����ȡ�Ѿ����������
	
	//delay_ms(4000);
	while(1)
	{
		//printf("11\r\n");
		//��ʱ��500ms���һ��2s����һ�ζ�ȡ����ָ��
		if(tim4_time >= query_interval)
		{
			printf("111\r\n");
			retry_to_lora++;
			memset(dat, '\0', sizeof(dat));			//���³�ʼ��
			sprintf(dat, "&&start$$");
			if(slave>slave_num)
			{
				slave = 1;
			}
			SendData_To_Slave(slave);
			//ÿ�η����������³�ʼ������
			All_Flag_Default();
			printf("����ָ����ӻ�%d, %s\r\n",slave, dat);
			
			//���ۻ�����5�η���δ���յ�ʱ�϶�lora�����������³�ʼ��
			if(retry_to_lora >= 5)
			{
//				LORA_Init();
				slave++;
				retry_to_lora = 0;
				if(slave == slave_num+1)
				{
					slave = 1;
				}
			}
			tim4_time = 0;
		}
		if(s2 == 2)
		{
			printf("22\r\n");
			Show_Str(50,25,BLUE,WHITE,"Send Data",16,0);
			u2_printf("oldaabc");								//����ָ���esp����esp�����������ݶ�����ʷ����
			///delay_ms(600);
			printf("������ʷ����\r\n");
			mf_open_sta = mf_open(HISTORYFILENAME,1);
			for(i = 10; i>0; i--)
			{
				mf_read_one_line(mf_size()/SAVE_DATA_LEN-i);
				delay_ms(800);
			}
			mf_close();
			u2_printf("nowdata");								//����ָ���esp����esp�����������ݶ���ʵʱ����
			delay_ms(300);
			//printf("%s", read_history_data);
			Show_Str(50,25,BLUE,WHITE,"Send Ok   ",16,0);
			All_Flag_Default();
		}
		//�Խ��յ������ݽ��д���
		if(s1 == 4)
		{
			printf("33\r\n");
//			printf("���յ�����\r\n");
			retry_to_lora = 0;
			while(USART3_RX_BUF[i] != '$'||USART3_RX_BUF[i+1] != '$')
			{
				if(USART3_RX_BUF[i] == '&'&&USART3_RX_BUF[i+1] == '&')
				{
					head = 1;
					
					i++;
					i++;
				}
				if(head == 1)
				{
					receive_data[pos] = USART3_RX_BUF[i];
					printf("%d ", receive_data[pos]);
					//printf("i:%d, pos:%d, dat:%d\r\n", i, pos, receive_data[pos]);
					pos++;
				}
				i++;
			}
			
			//�ڽ��չ����п��ܳ�����������޷����յ������ַ�,��Ῠ����ѭ����,���ڴ�ι��
			IWDG_Feed();
			GetData_From_Slave(slave);
			
			//ֻ���ڵ�һҳʱ����ʾ����
			if(PAGE == SLAVE_DATA_PAGE)
			{
				Show_Sensor_Data(Slave_Page);		
			}
			
			
			if(Slave_Page == slave_num)
				Slave_Page = 0;
			Slave_Page++;
			//���³�ʼ��
			All_Flag_Default();
		}
		if(receive_complete == 1)
		{
			printf("44\r\n");
			receive_complete = 0;
			//u2_printf("no\r\n");
			sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#%d#%d#%d#%1.3f#%1.3f#%1.3f#%d#%d#%d#%1.3f#%1.3f#%1.3f#%d#%d#%d#%1.3f#%1.3f#%1.3f#\r\n", 
			Slave_Avr[1].t, Slave_Avr[1].rh, Slave_Avr[1].light, Slave_Avr[1].sh, Slave_Avr[1].eco2 ,Slave_Avr[1].tvoc,
			Slave_Avr[2].t, Slave_Avr[2].rh, Slave_Avr[2].light, Slave_Avr[2].sh, Slave_Avr[2].eco2 ,Slave_Avr[2].tvoc,
			Slave_Avr[3].t, Slave_Avr[3].rh, Slave_Avr[3].light, Slave_Avr[3].sh, Slave_Avr[3].eco2 ,Slave_Avr[3].tvoc,
			Slave_Avr[4].t, Slave_Avr[4].rh, Slave_Avr[4].light, Slave_Avr[4].sh, Slave_Avr[4].eco2 ,Slave_Avr[4].tvoc);
			u2_printf("%s", write_file_dat);
		}
	}
}
//#22#37#125#0.706#0#1081921536#0#1075970048#0#0.000#0#0#0#0#0#0.000#0#0#0#0#0#0.000#0#0#off#

void All_Flag_Default()
{     
	str_len = 0;												//�ַ������ȸ�λ
	u2_str_len = 0;
	memset(USART3_RX_BUF, '\0', sizeof(USART3_RX_BUF));			//�ַ�����λ
	memset(receive_data, '\0', sizeof(receive_data));							//�ַ�����λ
	s1 = 0;														//�������ݽ�β�ж�
	s2 = 0;
	i = 0;														//u3�ַ�λ��
	pos = 0;													//receive_data�ַ�λ��			
	head = 0;													//�ж��Ƿ���յ�&&
	en_senddata = 0;											//ʹ�ܷ�������

}

void SendData_To_Slave(u8 n)
{
	printf("��ӻ�%d��������\r\n", n);
	Send_Data(Slave_channel_addr[n-1][1], Slave_channel_addr[n-1][2], Slave_channel_addr[n-1][3], dat);
//	switch(n)
//	{
//		printf("��ӻ�%d��������\r\n", n);
//		case 1: Send_Data(0x00, 0x01, 0x17, dat); break;
//		case 2: Send_Data(0x00, 0x02, 0x17, dat); break;
//		case 3: Send_Data(0x00, 0x03, 0x17, dat); break;
//		case 4: Send_Data(0x00, 0x04, 0x17, dat); break;
//		
//	}
}

void GetData_From_Slave(u8 n)
{
	//����λu8ƴ��һλu16
	light_value = receive_data[4];
	light_value = (light_value<<8)+ receive_data[6];
	soil_humidity = receive_data[8];
	soil_humidity = (soil_humidity<<8)+ receive_data[10];
	soil_humidity_value = soil_humidity*(3.3/4096);
	eco2 = receive_data[12];
	eco2 = (eco2<<8)+receive_data[14];
	eco2_value = eco2/10000.0;										//ʹ��floatʱ�����С���㲻ȻС���������ֲ���ʾ
	tvoc = receive_data[16]; 
	tvoc = (tvoc<<8)+receive_data[18];
	tvoc_value = tvoc/10000.0;										//ʹ��floatʱ�����С���㲻ȻС���������ֲ���ʾ

//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[1].t, Slave_Avr[1].rh, Slave_Avr[1].light, Slave_Avr[1].sh, Slave_Avr[1].eco2 ,Slave_Avr[1].tvoc);
//	printf("\r\n�ӻ�1:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[2].t, Slave_Avr[2].rh, Slave_Avr[2].light, Slave_Avr[2].sh, Slave_Avr[2].eco2 ,Slave_Avr[2].tvoc);
//	printf("\r\n�ӻ�2:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[3].t, Slave_Avr[3].rh, Slave_Avr[3].light, Slave_Avr[3].sh, Slave_Avr[3].eco2 ,Slave_Avr[3].tvoc);
//	printf("\r\n�ӻ�3:%s\r\n", write_file_dat);
	//�����ݴ�ŵ��ṹ����
	Slave_Avr[n].t = receive_data[0];
	Slave_Avr[n].rh = receive_data[2];
	Slave_Avr[n].light = light_value;
	Slave_Avr[n].sh = soil_humidity_value;
	Slave_Avr[n].eco2 = eco2_value;
	Slave_Avr[n].tvoc = tvoc_value;
	//�洢д�뵽sd��������
	Slave_Avr[n+3].t += Slave_Avr[n].t;
	Slave_Avr[n+3].rh += Slave_Avr[n].rh;
	Slave_Avr[n+3].light += Slave_Avr[n].light;
	Slave_Avr[n+3].sh += Slave_Avr[n].sh;
	Slave_Avr[n+3].eco2 += Slave_Avr[n].eco2;
	Slave_Avr[n+3].tvoc += Slave_Avr[n].tvoc;
	valid_data[n-1]++;			//��¼��Ч���ݴ����Ա���ƽ��ֵ
	if(n >= slave_num)
	{
		receive_complete = 1;
		slave = 0;
	}
	slave++;
//	printf("���յ��ӻ�%d����\r\n", n);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[4].t, Slave_Avr[4].rh, Slave_Avr[4].light, Slave_Avr[4].sh, Slave_Avr[4].eco2 ,Slave_Avr[4].tvoc);
//	printf("\r\n�ӻ�1:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[5].t, Slave_Avr[5].rh, Slave_Avr[5].light, Slave_Avr[5].sh, Slave_Avr[5].eco2 ,Slave_Avr[5].tvoc);
//	printf("\r\n�ӻ�2:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[6].t, Slave_Avr[6].rh, Slave_Avr[6].light, Slave_Avr[6].sh, Slave_Avr[6].eco2 ,Slave_Avr[6].tvoc);
//	printf("\r\n�ӻ�3:%s\r\n", write_file_dat);
}

void Show_Sensor_Data(u8 n)
{
//	LCD_direction(1);
//	Show_Str(50,100,BLUE,WHITE,"�¶�:     ",16,0);
//	Show_Str(50,175,BLUE,WHITE,"ʪ��:     ",16,0);
//	Show_Str(130,100,BLUE,WHITE,"CO2:     ",16,0);
//	Show_Str(130,175,BLUE,WHITE,"TVOC:    ",16,0);
//	Show_Str(210,100,BLUE,WHITE,"����ʪ��:     ",16,0);
//	Show_Str(210,175,BLUE,WHITE,"����:     ",16,0);
	
	//��������ʾ��oled

	//��ʾ�ӻ���ʶ
	Show_Str(20,30,BLUE,WHITE,"�ӻ�:",16,0);  LCD_ShowNum(45, 30,n, 3, 16);

	//�¶�
	sprintf(str_temp, "%d��", Slave_Avr[n].t);
	Show_Str(95,100,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(0, 2,str_temp,8);
	printf("\r\nstr_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	//ʪ��
	sprintf(str_temp, "%d%%", Slave_Avr[n].rh);
	Show_Str(95,175,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(0, 4,str_temp,8);
	printf("str_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	//CO2
	sprintf(str_temp, "%1.2f%%", Slave_Avr[n].eco2);
	Show_Str(165,100,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(60, 4,str_temp,8);
	printf("str_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	//TVOC
	sprintf(str_temp, "%1.2f%%", Slave_Avr[n].tvoc);
	Show_Str(165,175,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(60, 6,str_temp,8);
	printf("str_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	//����ʪ��
	sprintf(str_temp, "%2.1fV", Slave_Avr[n].sh);
	Show_Str(280,100,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(60, 2,str_temp,8);
	printf("str_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	//����
	sprintf(str_temp, "%2dLX", Slave_Avr[n].light);
	Show_Str(255,175,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(0, 6,str_temp,8);
	printf("str_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	
}

void Touch_Scan(void)
{
	u16 i;
	//LCD_direction(1);
	tp_dev.scan(0); 		 
	if(tp_dev.sta&TP_PRES_DOWN)			//������������
	{	
		Control_screen_on_off(1);
		//�����ļ�
		if((tp_dev.x<40&&tp_dev.x>5)&&(tp_dev.y>50&&tp_dev.y<70)&&PAGE == SLAVE_DATA_PAGE)
		{
			mf_scan_files("0:");
			delay_ms(300);
		}
		
		if((tp_dev.x<40&&tp_dev.x>5)&&(tp_dev.y>70&&tp_dev.y<90)&&PAGE == SLAVE_DATA_PAGE)
		{
//			mf_open_sta = mf_open(HISTORYFILENAME,2);
//			file_size = mf_size();
//			mf_lseek(file_size);
//			mf_puts("yyh���ݻ�");
//			mf_close();
		}
		if((tp_dev.x<40&&tp_dev.x>5)&&(tp_dev.y>90&&tp_dev.y<110)&&PAGE == SLAVE_DATA_PAGE)
		{
			u2_printf("oldaabc");								//����ָ���esp����esp�����������ݶ�����ʷ����
			delay_ms(600);
			printf("������ʷ����\r\n");
			int_history_num = mf_size();
			mf_open_sta = mf_open(HISTORYFILENAME,1);
			for(i = 10; i>0; i--)
			{
				mf_read_one_line(int_history_num/SAVE_DATA_LEN-i);
				delay_ms(800);
			}
			mf_close();
			u2_printf("nowdata");								//����ָ���esp����esp�����������ݶ���ʵʱ����
			delay_ms(300);
			//printf("%s", read_history_data);
			All_Flag_Default();
		}
		//�ж��Ƿ��ڵ�1ҳ���������
		if((tp_dev.x<SHEZHI_X +40&&tp_dev.x>SHEZHI_X)&&(tp_dev.y>SHEZHI_Y&&tp_dev.y<SHEZHI_Y+20)&&PAGE == SLAVE_DATA_PAGE)
		{
			//LCD_Clear(WHITE);
			SheZhi_View();
			//delay_ms(500);		//��ֹ��
		}
//		if(tp_dev.x<lcddev.width&&tp_dev.y<lcddev.height)
//		{	
//			printf("��Ļ������, ����x: %d, y: %d\r\n",tp_dev.x, tp_dev.y);
//			if(tp_dev.x>(lcddev.width-24)&&tp_dev.y<16)
//			{ 
//				POINT_COLOR=colorTemp;
//				LCD_Fill(lcddev.width-52,2,lcddev.width-50+20,18,POINT_COLOR); 
//			}                              
//			else if((tp_dev.x>(lcddev.width-60)&&tp_dev.x<(lcddev.width-50+20))&&tp_dev.y<20)
//			{
//			LCD_Fill(lcddev.width-52,2,lcddev.width-50+20,18,ColorTab[j%5]); 
//			POINT_COLOR=ColorTab[(j++)%5];
//			colorTemp=POINT_COLOR;
//			delay_ms(10);
//			}

//			else TP_Draw_Big_Point(tp_dev.x,tp_dev.y,POINT_COLOR);		//��ͼ	  			   
//		}
	}
	if(KEY_Scan()==1)	//KEY_RIGHT����,��ִ��У׼����
	{
//			key = 0;
		LCD_Clear(WHITE);//����
		TP_Adjust();  //��ĻУ׼ 
		TP_Save_Adjdata();	 
		DrawTestPage("����9:Touch(��KEY0У׼)      ");
		LCD_ShowString(lcddev.width-24,0,16,"RST",0);//��ʾ��������
		POINT_COLOR=colorTemp;
		LCD_Fill(lcddev.width-52,2,lcddev.width-50+20,18,POINT_COLOR); 
	}
}

u16 Get_History_Num()
{
	mf_open_sta = mf_open(HISTORYFILENAME,FA_CREATE_NEW);
	printf("mf_open_sta1:%s\r\n", mf_open_sta);
	mf_open_sta = mf_open(HISTORYFILENAME,1);
	printf("mf_open_sta2:%d\r\n", mf_open_sta);
	file_size = mf_size();
	if(file_size == 0)
	{
		mf_lseek(0);
		int_history_num = 1;
	}
	else
	{
		mf_lseek(file_size-20);
		mf_read(20,0);
		int_history_num = file_size/SAVE_DATA_LEN+1;
	}
	mf_close();
	
	printf("int_history_num:%d\r\n", int_history_num);
	return int_history_num;
}


void Save_History_10M()
{
	u8 i;
	printf("д����ʷ����\r\n");
	en_save_data = 1;
	//��ƽ��ֵ
	for(i = 4; i < 7; i++)
	{
		Slave_Avr[i].t = Slave_Avr[i].t/valid_data[i-4]; 
		Slave_Avr[i].rh = Slave_Avr[i].rh/valid_data[i-4];
		Slave_Avr[i].light = Slave_Avr[i].light/valid_data[i-4];
		Slave_Avr[i].sh = Slave_Avr[i].sh/valid_data[i-4];
		Slave_Avr[i].eco2 = Slave_Avr[i].eco2/valid_data[i-4];
		Slave_Avr[i].tvoc = Slave_Avr[i].tvoc/valid_data[i-4];
	}
	//ÿ�и�ʽ�̶��ҳ���Ϊ128�ֽ�
	sprintf(write_file_dat, "#(%6d)#%2d#%2d#%5d#%1.3f#%1.3f#%1.3f#%2d#%2d#%5d#%1.3f#%1.3f#%1.3f#%2d#%2d#%5d#%1.3f#%1.3f#%1.3f#%s#(%6d)#\r\n", 
	int_history_num,Slave_Avr[4].t, Slave_Avr[4].rh, Slave_Avr[4].light, Slave_Avr[4].sh, Slave_Avr[4].eco2 ,Slave_Avr[4].tvoc,
	Slave_Avr[5].t, Slave_Avr[5].rh, Slave_Avr[5].light, Slave_Avr[5].sh, Slave_Avr[5].eco2 ,Slave_Avr[5].tvoc,
	Slave_Avr[6].t, Slave_Avr[6].rh, Slave_Avr[6].light, Slave_Avr[6].sh, Slave_Avr[6].eco2 ,Slave_Avr[6].tvoc, 
//	Slave_Avr[4].t, Slave_Avr[4].rh, Slave_Avr[4].light, Slave_Avr[4].sh, Slave_Avr[4].eco2 ,Slave_Avr[4].tvoc,
	now_time,int_history_num);
	printf("data:%s\r\nint_history_num:%d\r\n", write_file_dat, int_history_num);
	int_history_num++;
	mf_open_sta = mf_open(HISTORYFILENAME,FA_CREATE_NEW);
	mf_open_sta = mf_open(HISTORYFILENAME,2);
	file_size = mf_size();
	mf_lseek(file_size);
	mf_puts(write_file_dat);
	mf_close();	
	
	//��λΪ0
	for(i = 4; i < 7; i++)
	{
		Slave_Avr[i].t = 0; 
		Slave_Avr[i].rh = 0;
		Slave_Avr[i].light = 0;
		Slave_Avr[i].sh = 0;
		Slave_Avr[i].eco2 = 0;
		Slave_Avr[i].tvoc = 0;
		valid_data[i-4] = 0;
	}
	
	
}

//��ȡ������Ϣÿ�ο���ʱ��ȡ
void Get_config_Data()
{
	u8 i, j;
	slave_num = AT24CXX_ReadLenByte(AT24CXX_SLAVE_NUM_ADDR, 1);
	query_interval = AT24CXX_ReadLenByte(AT24CXX_MASTER_QUERY_INTERVAL_TIME, 1);
	Screen_on_time = AT24CXX_ReadLenByte(AT24CXX_MASTER_SCREEN_ON_TIME, 1);
	for(i = 0; i < slave_num; i++)
	{
		for(j = 0; j < 4; j++)
		{
			Slave_channel_addr[i][j] = AT24CXX_ReadLenByte(AT24CXX_SLAVE_ADDR_ADDR+(i*4)+j, 1);
			printf("%d ", Slave_channel_addr[i][j]);
		}
		printf("\r\n");
	}
	
}








