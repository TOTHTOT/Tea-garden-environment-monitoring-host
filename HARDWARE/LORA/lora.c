#include "lora.h"
#include "usart3.h"
#include "usart.h"
#include "delay.h"
#include "string.h"

//char lora_tx_data[100];
//char lora_rx_data[100];

u8 LORA_Init(void)
{
//	u8 len, t, result;
//	u8 cmd[] = "AT";  
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOBʱ��
	
	//MD0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);

	
	//AUX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	
	LORA_Check();
	LORA_Set_Mode();
	
	return 1;
}

u8 AUX_Check()
{
	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))
	{
		//����1��ʾ��æ
		return 1;
	}
	else
	{
		//����0��ʾ��æ
		return 0;
	}
}

void LORA_Check()
{
	int LORA_State = 0;
	while(AUX_Check())
	{
		printf("LORA_Check��æ\r\n");
		delay_ms(50);
	}
	
	//��������ģʽ
	MD0 = 1;
	AUX = 0;
	printf("���LORA�Ƿ�����\r\n");
	delay_ms(20);
	while(!LORA_State)
	{
		u3_printf("AT\r\n");
		delay_ms(200);
		LORA_State = Check_LORA_Return_Is_OK();
		printf("Lora_State:%d\r\n", LORA_State);
		if(LORA_State == 2)
		{
			printf("���յ�ERROR\r\n");
			LORA_State = 0;
		}
		else if(LORA_State == 0)
		{
			printf("��δ������\r\n");
		}
		
	}
	printf("LORA���ɹ�\r\n");
}

void LORA_Set_Mode()
{
	
	//����Ƿ���æ
	while(AUX_Check())
	{
		printf("LORA_Set_Mode��æ\r\n");
		delay_ms(20);
	}
	
	//��������ģʽ
	MD0 = 1;
	AUX = 0;
	delay_ms(10);
	
	//���õ�ַ
	printf("\r\n���õ�ַ:");
	Send_Cmd("AT+ADDR=00,00");
	
	//��ȡ���ú�ĵ�ַ
	printf("\r\n��ȡ��ַ:");
	Quer_Cmd("AT+ADDR?");
	
	/**�����ŵ�������**/
	printf("\r\n�����ŵ�������:");
	Send_Cmd("AT+WLRATE=23,5");

	
	/**��ȡ�ŵ�**/
	printf("\r\n��ȡ�ŵ�:");
	Quer_Cmd("AT+WLRATE?");

	
	/**���÷��书��**/
	printf("\r\n���÷��书��:");
	Send_Cmd("AT+TPOWER=3");
	
	/**��ѯ���书��**/
	printf("\r\n��ѯ���书��:");
	Quer_Cmd("AT+TPOWER?");
	
	/**���ù���ģʽ**/
	printf("\r\n���ù���ģʽ");
	Send_Cmd("AT+CWMODE=0");
	
	/**��ѯ����ģʽ**/
	printf("\r\n��ѯ����ģʽ");
	Quer_Cmd("AT+CWMODE?");
	
	/**���÷���״̬**/
	printf("\r\n���÷���״̬");
	Send_Cmd("AT+TMODE=1");
	
	/**��ѯ����״̬**/
	printf("\r\n��ѯ����״̬");
	Quer_Cmd("AT+TMODE?");
	
	/**����˯��ʱ��**/
	printf("\r\n����˯��ʱ��");
	Send_Cmd("AT+WLTIME=0");
	
	/**��ѯ˯��ʱ��**/
	printf("\r\n��ѯ˯��ʱ��");
	Quer_Cmd("AT+WLTIME?");
	
	/**���ò����ʣ�����У��λ**/
	printf("\r\n���ò����ʣ�����У��λ");
	Send_Cmd("AT+UART=7,0");
	
	/**��ѯ�����ʣ�����У��λ**/
	printf("\r\n��ѯ�����ʣ�����У��λ");
	Quer_Cmd("AT+UART?");
	
	/**���ñ���ģʽ**/
	printf("\r\n���ñ���ģʽ");
	Send_Cmd("AT+FLASH=0");
	
	//�޲�ѯ����ģʽָ��
	/**��ѯ����ģʽ**/
	//printf("\r\n��ѯ����ģʽ");
	//Quer_Cmd("AT+FLASH?");

	
	printf("\r\n**********lora�������************\r\n");
	
	//����ͨ��ģʽ
	MD0 = 0;
	AUX = 0;
}

//����
void Send_Cmd(u8*cmd)
{
	u8 LORA_State = 0;
	
	LORA_State = 0;
	delay_ms(10);
	while(!LORA_State)
	{
		u3_printf("%s\r\n", cmd);
		delay_ms(20);
		LORA_State = Check_LORA_Return_Is_OK();
		printf("Lora_State:%d\r\n", LORA_State);
		if(LORA_State == 2)
		{
			printf("����ʱ���յ�ERROR\r\n");
			LORA_State = 0;
		}
		else if(LORA_State == 0)
		{
			printf("��δ������\r\n");
		}
	}
}

//��ѯ
void Quer_Cmd(u8 *cmd)
{
	u8 LORA_State = 0;
	
	LORA_State = 0;
	delay_ms(10);
	while(!LORA_State)
	{
		//printf("\r\n�ŵ�Ϊ:");
		u3_printf("%s\r\n",cmd);
		delay_ms(20);
		LORA_State = Check_LORA_Return_Is_OK();
		printf("Lora_State:%d\r\n", LORA_State);
		if(LORA_State == 2)
		{
			printf("��ѯʱ���յ�ERROR\r\n");
			LORA_State = 0;
		}
		else if(LORA_State == 0)
		{
			printf("��ѯ��δ������\r\n");
		}
	}
}


//��������
void  Send_Data(u8 addh, u8 addl,u8 channel,u8 *a)
{
	short i = 0;
	
	printf("addh:%d, addl:%d, ch:%d\r\n\r\n", addh, addl, channel);
	u3_send_byte(addh);
	u3_send_byte(addl);
	//USART_SendData(USART3,23);
	u3_send_byte(channel);		//������������� �����ܲ��� ��֪��Ϊʲô
	//��֪��Ϊʲô���ֿܷ��� ����channel�ؿ���
	//u3_printf("%x%x%x", 0x17);
	while(a[i] != '$')
	{
			u3_send_byte(a[i]);
			i++;
	}
	u3_send_byte('$');//������
	u3_send_byte('\r');
	u3_send_byte('\n');
}

//void Receive_Data(void)
//{
//	unsigned char i = 0;
//	int t1 = 0,t2 = 0;
//	while(1)
//	{
//		u3_get_byte(&i);
//		if(i=='0xff')
//		{
//			while(1)
//			{
//				if(u3_get_byte(&i) == 1 && i!='$')
//				{
//					lora_rx_data[t1++]=i;
//				}
//				else if(i=='$')
//					break;	 
//			}
//			while(1)
//			{
//					u1_send_byte(lora_rx_data[t2++]);
//					if(t1 == t2)
//					{
//						u1_send_byte('\r');
//						u1_send_byte('\n');
//						break;
//					}
//			}
//			break;
//		} 
//	}
//}
//��������ģʽ
void To_Configure(u8 *cmd, u8 *str)
{
	MD0 = 1;
	AUX = 0;
	
	u3_printf("%s\r\n",cmd);
}
