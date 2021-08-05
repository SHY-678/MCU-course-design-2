/*ds18b20����
**@CTBU^Shy^
**$version:v1.0
**#chinese_coding_standard:GB2312
*/
#include "ds18b20.h"
#include <reg52.h>
#include <intrins.h>
//***********************************************************************
sbit dq = P3^6;				//ds18b20�������߽ӿ�

//***********************************************************************
/*x10us��ʱ
**count:��ʱ 10 x count us
*/
void delay10us(unsigned char count)
{
	while (count--)
	{
		_nop_();
		_nop_();
	}
}
//***********************************************************************
/*ds18b20��ʼ��_��ѯ����״̬
**void
*/
bit ds18b20_init(void)
{
	bit response = 1;	 //ds18b20Ӧ���ʶ 0���� 1������
	
	dq = 0;				 //��λ
	delay10us(48);
	
	dq = 1;				 //�ͷ����ߣ�׼�����մ�������
	delay10us(5);		 //�ȴ�ds18b20���ش�������
	response = dq;		 //���մ����ź�
	delay10us(43);		 //�ͷ����߽���
	
	return response;
}
//***********************************************************************
/*ds18b20дһ���ֽ�����
**dat:Ҫд���8λ����
*/	
void ds18b20_write_byte(unsigned char dat)
{
	unsigned char i = 0;
	for(i = 0;i < 8;i++)
	{
		dq = 0;
		_nop_();
		_nop_();
		dq = dat&0x01; 		//�ӵ�λ��ʼ��
		delay10us(6);
		dq = 1;
		_nop_();
		dat>>=1;
	}
}
//***********************************************************************
/*ds18b20��һ���ֽ�����
**void
**���ض�ȡ����8λ����
*/	
unsigned char ds18b20_read_byte(void)
{
	unsigned char i = 0;
	unsigned char dat = 0;
	for(i = 0;i < 8;i++)
	{
		dat>>=1;
		dq = 0;
		_nop_();
		_nop_();
		dq = 1;
		if(dq)
			dat |= 0x80;	//�ӵ�λ��ʼ��
		delay10us(6);
	}
	return dat;
}
//***********************************************************************
/*ds18b20���ȿ���
**level��1-4 ��Ӧ0.5 0.25 0.125 0.0625����
*/	
void ds18b20_precision_set(unsigned char level)
{
	ds18b20_init();		  		   	   //��ʼ��DS18B20
	ds18b20_write_byte(0xcc);  		   //����rom
	ds18b20_write_byte(0x4e);  		   //д�ݴ���
	ds18b20_write_byte(0xa0);  		   //TH 160
	ds18b20_write_byte(0x00);  		   //TL 0
	switch(level)	//���ȿ���
	{
		case 1:		//0x1f : 0.5000  93.75ms
			ds18b20_write_byte(0x1f);
			break;
		case 2:		//0x3f : 0.2500  187.5ms
			ds18b20_write_byte(0x3f);
			break;
		case 3:		//0x5f : 0.1250  375ms
			ds18b20_write_byte(0x5f);
			break;
		case 4:		//0x7f : 0.0625  750ms
			ds18b20_write_byte(0x7f);
			break;
		default:
			ds18b20_write_byte(0x7f);
			break;
	}					
}
//***********************************************************************
/*ds18b20��ȡ�¶�
**void
**�Ŵ�100�����¶�
*/	
int ds18b20_read_temperature(void)
{
	int temperature = 0;
	unsigned char lsb,msb;
	float f_temperature;
	
	ds18b20_init();
	ds18b20_write_byte(0xcc); 		//����rom
	ds18b20_write_byte(0x44); 		//����һ���¶�ת��
	
	ds18b20_init();
	ds18b20_write_byte(0xcc); 		//����rom
	ds18b20_write_byte(0xbe); 		//���Ĵ���
	lsb = ds18b20_read_byte(); 		//��8λ
	msb = ds18b20_read_byte(); 		//��8λ
	
	temperature |= msb; 			//��8λ
	temperature<<=8;
	temperature |= lsb; 			//��8λ
	
	f_temperature = temperature * 0.0625;
	temperature = f_temperature * 10;
	
	return temperature;
}