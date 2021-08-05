/*ds18b20驱动
**@CTBU^Shy^
**$version:v1.0
**#chinese_coding_standard:GB2312
*/
#include "ds18b20.h"
#include <reg52.h>
#include <intrins.h>
//***********************************************************************
sbit dq = P3^6;				//ds18b20数据总线接口

//***********************************************************************
/*x10us延时
**count:延时 10 x count us
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
/*ds18b20初始化_查询存在状态
**void
*/
bit ds18b20_init(void)
{
	bit response = 1;	 //ds18b20应答标识 0存在 1不存在
	
	dq = 0;				 //复位
	delay10us(48);
	
	dq = 1;				 //释放总线，准备接收存在脉冲
	delay10us(5);		 //等待ds18b20返回存在脉冲
	response = dq;		 //接收存在信号
	delay10us(43);		 //释放总线结束
	
	return response;
}
//***********************************************************************
/*ds18b20写一个字节数据
**dat:要写入的8位数据
*/	
void ds18b20_write_byte(unsigned char dat)
{
	unsigned char i = 0;
	for(i = 0;i < 8;i++)
	{
		dq = 0;
		_nop_();
		_nop_();
		dq = dat&0x01; 		//从低位开始传
		delay10us(6);
		dq = 1;
		_nop_();
		dat>>=1;
	}
}
//***********************************************************************
/*ds18b20读一个字节数据
**void
**返回读取到的8位数据
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
			dat |= 0x80;	//从低位开始读
		delay10us(6);
	}
	return dat;
}
//***********************************************************************
/*ds18b20精度控制
**level：1-4 对应0.5 0.25 0.125 0.0625精度
*/	
void ds18b20_precision_set(unsigned char level)
{
	ds18b20_init();		  		   	   //初始化DS18B20
	ds18b20_write_byte(0xcc);  		   //跳过rom
	ds18b20_write_byte(0x4e);  		   //写暂存器
	ds18b20_write_byte(0xa0);  		   //TH 160
	ds18b20_write_byte(0x00);  		   //TL 0
	switch(level)	//精度控制
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
/*ds18b20读取温度
**void
**放大100倍的温度
*/	
int ds18b20_read_temperature(void)
{
	int temperature = 0;
	unsigned char lsb,msb;
	float f_temperature;
	
	ds18b20_init();
	ds18b20_write_byte(0xcc); 		//跳过rom
	ds18b20_write_byte(0x44); 		//启动一次温度转换
	
	ds18b20_init();
	ds18b20_write_byte(0xcc); 		//跳过rom
	ds18b20_write_byte(0xbe); 		//读寄存器
	lsb = ds18b20_read_byte(); 		//低8位
	msb = ds18b20_read_byte(); 		//高8位
	
	temperature |= msb; 			//高8位
	temperature<<=8;
	temperature |= lsb; 			//低8位
	
	f_temperature = temperature * 0.0625;
	temperature = f_temperature * 10;
	
	return temperature;
}