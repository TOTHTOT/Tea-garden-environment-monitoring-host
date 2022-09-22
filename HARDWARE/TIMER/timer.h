#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

extern u8 tim4_time;			//���Ʒ��ͼ��ʱ��
extern u8 TP_Countdown;			//��ĻϨ���ʱ
extern u8 now_time[];			//����ʵʱʱ��
//struct _TDateTime TDATATIME;		//ʱ��ṹ��
extern u8 HISTORYFILENAME[25];			//�ļ�����
extern u8 Data_storage_interval;   //��������ʱ����
typedef struct 
{
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u8  msec;
}_TDateTime;


void TIM1_PWM_Init(u16 arr,u16 psc);
void TIM3_Cap_Init(u16 arr,u16 psc);
void system_time_increase(void);
void Set_Time(void);
#endif
