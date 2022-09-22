#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

extern u8 tim4_time;			//控制发送间隔时间
extern u8 TP_Countdown;			//屏幕熄灭计时
extern u8 now_time[];			//储存实时时间
//struct _TDateTime TDATATIME;		//时间结构体
extern u8 HISTORYFILENAME[25];			//文件名称
extern u8 Data_storage_interval;   //储存数据时间间隔
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
