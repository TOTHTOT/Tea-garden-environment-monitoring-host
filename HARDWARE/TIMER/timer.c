#include "timer.h"
#include "led.h"
#include "usart.h"
#include "sys.h"
#include "lcd.h"
#include "string.h"
#include "gui.h"
#include "usart2.h"
#include "delay.h"
#include "main.h"


u8 tim4_time = 0;
u8 flag = 0;
u8 TP_Countdown = 0;

u8 now_time[20];
u8 HISTORYFILENAME[25];
u8 Data_storage_interval = 10;
_TDateTime TDATATIME;
//_TDateTime TIME;
void TIM3_Cap_Init(u16 arr, u16 psc)
{
//	TIM_ICInitTypeDef TIM3_ICInitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//��ʼ����ʱ��3 tim3
	TIM_DeInit(TIM3);
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
//	//��ʼ��tim2���벶�����
//	
//	TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2;
//	TIM3_ICInitStructure.TIM_ICFilter = 0x00;
//	TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
//	TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//	TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//	TIM_ICInit(TIM3, &TIM3_ICInitStructure);
//	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
	
	//STM32��ʼ����Ϻ��һ��������ʱ��ֱ�ӽ����жϣ�
	//ԭ���Ƕ�ʱ���ļĴ����жϱ�ʶû�б��������
	//���ʹ�ñ�׼�⣬����TIM_ClearITPendingBit(TIM1, TIM_IT_Update)����жϱ�ǣ�
	//Ȼ��ʹ�ܶ�ʱ���жϣ����������ʱ����
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);			//��������ж�,CC1IE�����ж�

	TIM_Cmd(TIM3, ENABLE);																		//ʹ�ܶ�ʱ��2
	
}


//u16 TIM3CH2_CAPTURE_STA = 0;		//���벶��״̬
//u16 TIM3CH2_CAPTURE_VAL = 0;			//���벶��ֵ

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
	{
		tim4_time++;
		system_time_increase();
		if(LCD_Sta == 1)
			TP_Countdown++;
		
		if(TP_Countdown == Screen_on_time)
		{
			TP_Countdown = 0;
			Control_screen_on_off(0);
		}
//		if(flag == 0)
//		{
//			SheZhi_View();
//			flag = 1;
//		}
//		else if(flag == 1)
//		{
//			flag = 0;
//			Rotate_Test();
//		}
		LED0 =~ LED0;
		//printf("���붨ʱ���ж�\r\n");
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
		
		
	}
}
void Set_Time(void)
{
	u8 retury = 0;
	u2_printf("getnowtime");
	delay_ms(10);
	while(s2 != 5)
	{
		if(retury%50==0)
		{
			u2_printf("getnowtime");
		}
		if(retury>20)
		{
			break;
		}
		printf("retury:%d\r\n", retury);
		retury++;
		delay_ms(50);
	}
	if(retury<20)
	{
		TDATATIME.year = (time_data_esp[2]-48)*1000+(time_data_esp[3]-48)*100+(time_data_esp[4]-48)*10+(time_data_esp[5]-48);
		TDATATIME.month = (time_data_esp[7]-48)*10+(time_data_esp[8]-48);
		TDATATIME.day = (time_data_esp[10]-48)*10+(time_data_esp[11]-48);
		TDATATIME.hour = (time_data_esp[13]-48)*10+(time_data_esp[14]-48);
		TDATATIME.minute = (time_data_esp[16]-48)*10+(time_data_esp[17]-48);
		TDATATIME.second= (time_data_esp[19]-48)*10+(time_data_esp[20]-48);
		printf("time :%d %d %d %c\r\n", time_data_esp[2],time_data_esp[3], time_data_esp[4], time_data_esp[5]);
		sprintf(now_time, "%4d/%2d/%2d/%2d:%2d", TDATATIME.year, TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute);
		//	printf("ʱ��:%s\r\n", now_time);
		Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,now_time,16,0);//������ʾ�޸��Ǽ���Ҫ��ʾλ�õ��ַ���ʾǮ���ԭ��ʾ����ʾ����������� ���������ԭ�ȵ��ַ������	
		//ÿ�ο���ʱ������ʷ�ļ����ļ�����
		sprintf(HISTORYFILENAME, "0:/%d%d%d.TXT", TDATATIME.year, TDATATIME.month, TDATATIME.day);	
		printf("%s\r\n", HISTORYFILENAME);
	}
	else
	{
		
		printf("û���յ�esp������ʱ������!!!\r\n");
		TDATATIME.year = 2022;
		TDATATIME.month = 1;
		TDATATIME.day = 1;
		TDATATIME.hour = 0;
		TDATATIME.minute = 0;
		TDATATIME.second=0;
		sprintf(HISTORYFILENAME, "0:/%d%d%d.TXT", TDATATIME.year, TDATATIME.month, TDATATIME.day);	
		printf("%s\r\n", HISTORYFILENAME);
		sprintf(now_time, "%4d/%2d/%2d/%2d:%2d", TDATATIME.year, TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute);
		//	printf("ʱ��:%s\r\n", now_time);
		Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,now_time,16,0);//������ʾ�޸��Ǽ���Ҫ��ʾλ�õ��ַ���ʾǮ���ԭ��ʾ����ʾ����������� ���������ԭ�ȵ��ַ������	
	}
	s2 = 0;
	memset(USART2_RX_BUF, '\0', sizeof(USART2_RX_BUF));			//�ַ�����λ
	u2_printf("setnowdata");
}
void system_time_increase(void)
{
    uint8_t month_day_tab[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};  //ÿ�µ�����
    TDATATIME.msec++;
    if(TDATATIME.msec >= 2)  //1ms�����ж�һ�Σ�1000�ξ���1ms
    {
        TDATATIME.msec = 0L;
        TDATATIME.second++;//���λ
        if(TDATATIME.second >= 60)
        {
		sprintf(now_time, "%4d/%2d/%2d/%2d:%2d", TDATATIME.year, TDATATIME.month, TDATATIME.day, TDATATIME.hour, TDATATIME.minute);
		//	printf("ʱ��:%s\r\n", now_time);
		Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,now_time,16,0);//������ʾ�޸��Ǽ���Ҫ��ʾλ�õ��ַ���ʾǮ���ԭ��ʾ����ʾ����������� ���������ԭ�ȵ��ַ������	
            TDATATIME.second = 0;
            TDATATIME.minute++;//���ӽ�λ
			if(TDATATIME.minute%Data_storage_interval == 0)
				Save_History_10M();
            if(TDATATIME.minute >= 60)
            {
                TDATATIME.minute = 0;
                TDATATIME.hour++;//Сʱ��λ
                if(TDATATIME.hour >= 24)
                {
                    TDATATIME.hour = 0;
                    TDATATIME.day++;//���λ
					sprintf(HISTORYFILENAME, "0:/%d%d%d.TXT", TDATATIME.year, TDATATIME.month, TDATATIME.day);	
                    if(TDATATIME.day > (((TDATATIME.year%4 == 0) && (TDATATIME.month == 2))?
                                        month_day_tab[TDATATIME.month]+1:month_day_tab[TDATATIME.month]) )
                    {
                        TDATATIME.day = 1;
						sprintf(HISTORYFILENAME, "0:/%d%d%d.TXT", TDATATIME.year, TDATATIME.month, TDATATIME.day);
                        TDATATIME.month++;//�½�λ
                        if(TDATATIME.month > 12)
                        {
                            TDATATIME.month = 1;
                            TDATATIME.year++;//���λ
                            if(TDATATIME.year > 99)
                            {
                                TDATATIME.year = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}