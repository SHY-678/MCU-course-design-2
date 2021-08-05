/*8�ι��������������-��̬
**@CTBU^Shy^
**$version:v1.0
**#chinese_coding_standard:GB2312
*/
#include "nixie_tube.h"
#include <intrins.h>
#include <reg52.h>
//***********************************************************************
#define  data_port    P1    //���ݴ��Ͷ˿�

sbit dat_latch = P0^0;		//74hc573�����
sbit position_latch = P0^1; //74hc573�����

//***********************************************************************
/*1ms��ʱ
**void
*/
void delay200us()					//@12.000MHz
{
	unsigned char i, j;
	i = 2;
	j = 45;
	do
	{
		while (--j);
	} while (--i);
}
//***********************************************************************
/*8�ι������������ʾ
**position:0-7λѡ seg_dat:����������
*/
void single_tube_display(unsigned char position,unsigned char seg_dat)
{
	data_port = 0x01<<position; //λѡ����
	position_latch = 1;
	position_latch = 0;
	
	data_port = seg_dat;		//��������
	dat_latch = 1;
	dat_latch = 0;
	delay200us();				//��֤����
	delay200us();				//��֤����
	delay200us();				//��֤����
	data_port = 0xff;			//����
	dat_latch = 1;
	dat_latch = 0;
}

