#ifndef __MAIN_H
#define __MAIN_H

#include "sys.h"

extern u8 PAGE;
extern u8 Screen_on_time;		//��Ļ����ʱ��
extern u8 slave_num;												//�ӻ�����,���������������,Ĭ��Ϊ3
extern u8 query_interval;											//��ѯ���ʱ����СΪ1(500ms), �������޸�
extern u8 Slave_channel_addr[10][4];								//10���ӻ��ĵ�ַ���ŵ�

void All_Flag_Default(void);
void SendData_To_Slave(u8 n);
void GetData_From_Slave(u8 n);
void Show_Sensor_Data(u8 n);
void Touch_Scan(void);
u16 Get_History_Num(void);
void Save_History_10M(void);
void Get_config_Data(void);

#endif
