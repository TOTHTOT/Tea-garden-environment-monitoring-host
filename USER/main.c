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

u8 slave_num = 1;												//从机数量,在设置里调整数量,默认为1
u8 query_interval = 2;											//轮询间隔时间最小为1(500ms), 设置里修改
u8 Screen_on_time = 60;											//屏幕休眠时间倒计时, 设置里修改
u8 Slave_channel_addr[10][4];									//10个从机的地址及信道
u8 slave = 1, mf_open_sta;										//往哪个从机发送消息
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
extern _TDateTime TDATATIME;		//时间结构体
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

	SystemInit();												 //系统初始化时调用 
	delay_init();	    										 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 		  	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 									   	//串口初始化为115200
	LED_Init();
	USART2_Init(115200);										//和esp8266通信
	usart3_init(115200);			
	LORA_Init();
	AT24CXX_Init();												//初始化at24c02
//	EXTIX_Init();
	exfuns_init();												//为fatfs相关变量申请内存
//	IWDG_Init(6,4000); 											//与分频数为 256,重载值为 625,溢出时间为 25.6s
	LCD_Init();	   												//液晶屏初始化
	TP_Init();
	KEY_Init();
	mem_init();													//初始化内存池	
	while(SD_Initialize())										//检测不到SD卡
	{
		
		Show_Str(50,25,BLUE,WHITE,"SD Card Erro!!!",16,0);
		delay_ms(500);					
		Show_Str(50,25,BLUE,WHITE,"Please Check!!!",16,0);
		delay_ms(500);
		LED0=!LED0;//DS0闪烁
	}

	Show_Str(50,50,BLUE,WHITE,"SD Card OK!!!",16,0);
	exfuns_init();												//为fatfs相关变量申请内存				 
	f_mount(fs,"0:",1); 										//挂载SD卡 
	Rotate_Test();												//初始化界面
	Set_Time();													//初始化时间
	Get_config_Data();											//从at24c02获取配置信息
	while(AUX_Check())											//lora判忙
	{
		printf("LORA正忙\r\n");
		delay_ms(500);
	}
	printf("LORA正常\r\n");
	Get_History_Num();											//每次开机读取已经保存的条数
	
	//delay_ms(4000);
	while(1)
	{
		//printf("11\r\n");
		//定时器500ms溢出一次2s发送一次读取数据指令
		if(tim4_time >= query_interval)
		{
			printf("111\r\n");
			retry_to_lora++;
			memset(dat, '\0', sizeof(dat));			//重新初始化
			sprintf(dat, "&&start$$");
			if(slave>slave_num)
			{
				slave = 1;
			}
			SendData_To_Slave(slave);
			//每次发送数据重新初始化变量
			All_Flag_Default();
			printf("发送指令给从机%d, %s\r\n",slave, dat);
			
			//当累积发送5次发送未接收到时断定lora出问题了重新初始化
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
			u2_printf("oldaabc");								//发送指令给esp告诉esp接下来的数据都是历史数据
			///delay_ms(600);
			printf("发送历史数据\r\n");
			mf_open_sta = mf_open(HISTORYFILENAME,1);
			for(i = 10; i>0; i--)
			{
				mf_read_one_line(mf_size()/SAVE_DATA_LEN-i);
				delay_ms(800);
			}
			mf_close();
			u2_printf("nowdata");								//发送指令给esp告诉esp接下来的数据都是实时数据
			delay_ms(300);
			//printf("%s", read_history_data);
			Show_Str(50,25,BLUE,WHITE,"Send Ok   ",16,0);
			All_Flag_Default();
		}
		//对接收到的数据进行处理
		if(s1 == 4)
		{
			printf("33\r\n");
//			printf("接收到数据\r\n");
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
			
			//在接收过程中可能出现意外情况无法接收到结束字符,则会卡死在循环里,故在此喂狗
			IWDG_Feed();
			GetData_From_Slave(slave);
			
			//只有在第一页时才显示数据
			if(PAGE == SLAVE_DATA_PAGE)
			{
				Show_Sensor_Data(Slave_Page);		
			}
			
			
			if(Slave_Page == slave_num)
				Slave_Page = 0;
			Slave_Page++;
			//重新初始化
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
	str_len = 0;												//字符串长度复位
	u2_str_len = 0;
	memset(USART3_RX_BUF, '\0', sizeof(USART3_RX_BUF));			//字符串复位
	memset(receive_data, '\0', sizeof(receive_data));							//字符串复位
	s1 = 0;														//接收数据结尾判断
	s2 = 0;
	i = 0;														//u3字符位置
	pos = 0;													//receive_data字符位置			
	head = 0;													//判断是否接收到&&
	en_senddata = 0;											//使能发送数据

}

void SendData_To_Slave(u8 n)
{
	printf("向从机%d发送数据\r\n", n);
	Send_Data(Slave_channel_addr[n-1][1], Slave_channel_addr[n-1][2], Slave_channel_addr[n-1][3], dat);
//	switch(n)
//	{
//		printf("向从机%d发送数据\r\n", n);
//		case 1: Send_Data(0x00, 0x01, 0x17, dat); break;
//		case 2: Send_Data(0x00, 0x02, 0x17, dat); break;
//		case 3: Send_Data(0x00, 0x03, 0x17, dat); break;
//		case 4: Send_Data(0x00, 0x04, 0x17, dat); break;
//		
//	}
}

void GetData_From_Slave(u8 n)
{
	//将两位u8拼成一位u16
	light_value = receive_data[4];
	light_value = (light_value<<8)+ receive_data[6];
	soil_humidity = receive_data[8];
	soil_humidity = (soil_humidity<<8)+ receive_data[10];
	soil_humidity_value = soil_humidity*(3.3/4096);
	eco2 = receive_data[12];
	eco2 = (eco2<<8)+receive_data[14];
	eco2_value = eco2/10000.0;										//使用float时必须加小数点不然小数点后的数字不显示
	tvoc = receive_data[16]; 
	tvoc = (tvoc<<8)+receive_data[18];
	tvoc_value = tvoc/10000.0;										//使用float时必须加小数点不然小数点后的数字不显示

//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[1].t, Slave_Avr[1].rh, Slave_Avr[1].light, Slave_Avr[1].sh, Slave_Avr[1].eco2 ,Slave_Avr[1].tvoc);
//	printf("\r\n从机1:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[2].t, Slave_Avr[2].rh, Slave_Avr[2].light, Slave_Avr[2].sh, Slave_Avr[2].eco2 ,Slave_Avr[2].tvoc);
//	printf("\r\n从机2:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[3].t, Slave_Avr[3].rh, Slave_Avr[3].light, Slave_Avr[3].sh, Slave_Avr[3].eco2 ,Slave_Avr[3].tvoc);
//	printf("\r\n从机3:%s\r\n", write_file_dat);
	//将数据存放到结构体中
	Slave_Avr[n].t = receive_data[0];
	Slave_Avr[n].rh = receive_data[2];
	Slave_Avr[n].light = light_value;
	Slave_Avr[n].sh = soil_humidity_value;
	Slave_Avr[n].eco2 = eco2_value;
	Slave_Avr[n].tvoc = tvoc_value;
	//存储写入到sd卡的数据
	Slave_Avr[n+3].t += Slave_Avr[n].t;
	Slave_Avr[n+3].rh += Slave_Avr[n].rh;
	Slave_Avr[n+3].light += Slave_Avr[n].light;
	Slave_Avr[n+3].sh += Slave_Avr[n].sh;
	Slave_Avr[n+3].eco2 += Slave_Avr[n].eco2;
	Slave_Avr[n+3].tvoc += Slave_Avr[n].tvoc;
	valid_data[n-1]++;			//记录有效数据次数以便求平均值
	if(n >= slave_num)
	{
		receive_complete = 1;
		slave = 0;
	}
	slave++;
//	printf("接收到从机%d数据\r\n", n);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[4].t, Slave_Avr[4].rh, Slave_Avr[4].light, Slave_Avr[4].sh, Slave_Avr[4].eco2 ,Slave_Avr[4].tvoc);
//	printf("\r\n从机1:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[5].t, Slave_Avr[5].rh, Slave_Avr[5].light, Slave_Avr[5].sh, Slave_Avr[5].eco2 ,Slave_Avr[5].tvoc);
//	printf("\r\n从机2:%s\r\n", write_file_dat);
//	sprintf(write_file_dat, "#%d#%d#%d#%1.3f#%1.3f#%1.3f#", Slave_Avr[6].t, Slave_Avr[6].rh, Slave_Avr[6].light, Slave_Avr[6].sh, Slave_Avr[6].eco2 ,Slave_Avr[6].tvoc);
//	printf("\r\n从机3:%s\r\n", write_file_dat);
}

void Show_Sensor_Data(u8 n)
{
//	LCD_direction(1);
//	Show_Str(50,100,BLUE,WHITE,"温度:     ",16,0);
//	Show_Str(50,175,BLUE,WHITE,"湿度:     ",16,0);
//	Show_Str(130,100,BLUE,WHITE,"CO2:     ",16,0);
//	Show_Str(130,175,BLUE,WHITE,"TVOC:    ",16,0);
//	Show_Str(210,100,BLUE,WHITE,"土壤湿度:     ",16,0);
//	Show_Str(210,175,BLUE,WHITE,"光照:     ",16,0);
	
	//将数据显示到oled

	//显示从机标识
	Show_Str(20,30,BLUE,WHITE,"从机:",16,0);  LCD_ShowNum(45, 30,n, 3, 16);

	//温度
	sprintf(str_temp, "%d℃", Slave_Avr[n].t);
	Show_Str(95,100,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(0, 2,str_temp,8);
	printf("\r\nstr_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	//湿度
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
	//土壤湿度
	sprintf(str_temp, "%2.1fV", Slave_Avr[n].sh);
	Show_Str(280,100,BLUE,WHITE,str_temp,16,0);
	//OLED_ShowString(60, 2,str_temp,8);
	printf("str_temp:%s\r\n", str_temp);
	memset(str_temp, '\0', sizeof(str_temp));
	//光照
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
	if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
	{	
		Control_screen_on_off(1);
		//遍历文件
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
//			mf_puts("yyh杨逸辉");
//			mf_close();
		}
		if((tp_dev.x<40&&tp_dev.x>5)&&(tp_dev.y>90&&tp_dev.y<110)&&PAGE == SLAVE_DATA_PAGE)
		{
			u2_printf("oldaabc");								//发送指令给esp告诉esp接下来的数据都是历史数据
			delay_ms(600);
			printf("发送历史数据\r\n");
			int_history_num = mf_size();
			mf_open_sta = mf_open(HISTORYFILENAME,1);
			for(i = 10; i>0; i--)
			{
				mf_read_one_line(int_history_num/SAVE_DATA_LEN-i);
				delay_ms(800);
			}
			mf_close();
			u2_printf("nowdata");								//发送指令给esp告诉esp接下来的数据都是实时数据
			delay_ms(300);
			//printf("%s", read_history_data);
			All_Flag_Default();
		}
		//判断是否在第1页点击了设置
		if((tp_dev.x<SHEZHI_X +40&&tp_dev.x>SHEZHI_X)&&(tp_dev.y>SHEZHI_Y&&tp_dev.y<SHEZHI_Y+20)&&PAGE == SLAVE_DATA_PAGE)
		{
			//LCD_Clear(WHITE);
			SheZhi_View();
			//delay_ms(500);		//防止误触
		}
//		if(tp_dev.x<lcddev.width&&tp_dev.y<lcddev.height)
//		{	
//			printf("屏幕被按下, 坐标x: %d, y: %d\r\n",tp_dev.x, tp_dev.y);
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

//			else TP_Draw_Big_Point(tp_dev.x,tp_dev.y,POINT_COLOR);		//画图	  			   
//		}
	}
	if(KEY_Scan()==1)	//KEY_RIGHT按下,则执行校准程序
	{
//			key = 0;
		LCD_Clear(WHITE);//清屏
		TP_Adjust();  //屏幕校准 
		TP_Save_Adjdata();	 
		DrawTestPage("测试9:Touch(按KEY0校准)      ");
		LCD_ShowString(lcddev.width-24,0,16,"RST",0);//显示清屏区域
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
	printf("写入历史数据\r\n");
	en_save_data = 1;
	//求平均值
	for(i = 4; i < 7; i++)
	{
		Slave_Avr[i].t = Slave_Avr[i].t/valid_data[i-4]; 
		Slave_Avr[i].rh = Slave_Avr[i].rh/valid_data[i-4];
		Slave_Avr[i].light = Slave_Avr[i].light/valid_data[i-4];
		Slave_Avr[i].sh = Slave_Avr[i].sh/valid_data[i-4];
		Slave_Avr[i].eco2 = Slave_Avr[i].eco2/valid_data[i-4];
		Slave_Avr[i].tvoc = Slave_Avr[i].tvoc/valid_data[i-4];
	}
	//每行格式固定且长度为128字节
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
	
	//复位为0
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

//获取配置信息每次开机时获取
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








