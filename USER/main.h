#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"

extern u8 PAGE;
extern u8 Screen_on_time;		//屏幕亮起时间
extern u8 slave_num;												//从机数量,在设置里调整数量,默认为3
extern u8 query_interval;											//轮询间隔时间最小为1(500ms), 设置里修改
extern u8 Slave_channel_addr[10][4];								//10个从机的地址及信道

void All_Flag_Default(void);
void SendData_To_Slave(u8 n);
void GetData_From_Slave(u8 n);
void Show_Sensor_Data(u8 n);
void Touch_Scan(void);
u16 Get_History_Num(void);
void Save_History_10M(void);
void Get_config_Data(void);

#endif
