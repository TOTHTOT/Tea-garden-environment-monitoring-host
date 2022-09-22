#ifndef __LORA_H
#define __LORA_H

#include "sys.h"

#define MD0 PBout(1)
#define AUX PBout(0)

u8 LORA_Init(void);
//extern char lora_tx_data[100];
//extern char lora_rx_data[100];

void To_Configure(u8 *cmd, u8 *str);
void LORA_Check(void);
void LORA_Set_Mode(void);
void Send_Cmd(u8*cmd);
void Quer_Cmd(u8 *cmd);
void  Send_Data(u8 addh, u8 addl,u8 channel,u8 *a);
u8 AUX_Check(void);

#endif
