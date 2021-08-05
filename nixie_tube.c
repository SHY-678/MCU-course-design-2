/*8段共阳极数码管驱动-动态
**@CTBU^Shy^
**$version:v1.0
**#chinese_coding_standard:GB2312
*/
#include "nixie_tube.h"
#include <intrins.h>
#include <reg52.h>
//***********************************************************************
#define  data_port    P1    //数据传送端口

sbit dat_latch = P0^0;		//74hc573锁存端
sbit position_latch = P0^1; //74hc573锁存端

//***********************************************************************
/*1ms延时
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
/*8段共阳极数码管显示
**position:0-7位选 seg_dat:共阳极断码
*/
void single_tube_display(unsigned char position,unsigned char seg_dat)
{
	data_port = 0x01<<position; //位选数据
	position_latch = 1;
	position_latch = 0;
	
	data_port = seg_dat;		//断码数据
	dat_latch = 1;
	dat_latch = 0;
	delay200us();				//保证亮度
	delay200us();				//保证亮度
	delay200us();				//保证亮度
	data_port = 0xff;			//消隐
	dat_latch = 1;
	dat_latch = 0;
}

