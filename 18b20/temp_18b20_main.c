/*ds18b20温度显示
**@CTBU^Shy^
**$version:v1.1
**#chinese_coding_standard:GB2312
*/
#include "ds18b20.h"
#include "nixie_tube.h"
#include <reg52.h>
//***********************************************************************
#define precision_level     1        		//等级1 - 0.5分辨率
#define show_temperature    0
#define show_priod  		1
#define show_upper_limit 	2				//12.7
#define min_sampling_priod 100
#define max_sampling_priod 60000			//到60秒   
#define max_temperature    120
sbit key_priod = P2^0;
sbit key_inc   = P2^1;
sbit key_dec   = P2^2;
sbit key_ok    = P2^3;
sbit alarm     = P3^4;
//***********************************************************************
unsigned char run_state = show_temperature;
float temperature_alarm_upper_limit = 17.5;
unsigned char code seg_tab[] = 
					{0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,
			  // seg//'0'  '1'  '2'  '3'  '4'  '5'  '6'  '7'  '8'  '9'
			  //code// 0    1    2    3    4    5    6    7    8    9
					 0x88,0x83,0xc6,0xa1,0xc1,0x89,0xff,0x9c,0x8c};
			  // seg//'a'  'b'  'c'  'd'  'U'  'H'  ' '  '*'  'p' 
			  //code// a    b    c    d    e    f    10  11   12
		 
int temp = 0;
int count = 0;	
unsigned char blink_position = 4;
unsigned int sampling_priod = min_sampling_priod;	//采样周期ms 
bit key_pressed = 0;	
unsigned char tc = 0;					 
//***********************************************************************	
void temp_display(int temperature);
void upper_limit_display(float upper);
void sampling_priod_display(unsigned int sampling_priod);
void timer0_init(void);	//10ms	
//***********************************************************************
void main(void)
{
	EA = 1;
	ET0 =1;
	ds18b20_init();		  		   	 		  //初始化DS18B20
	ds18b20_precision_set(precision_level);   //0.5
	temp = ds18b20_read_temperature();
	timer0_init();
	P2 = 0xff;
	while(1)
	{
		switch(run_state)
		{
			case show_temperature:
				
				if(count > sampling_priod/20)
				{
					temp = ds18b20_read_temperature();
					count = 0;
					if(temp>temperature_alarm_upper_limit*10)
					alarm=0;
				else
					alarm=1;
				}
				
				/*功能键检测*/
				if(key_pressed==0&&key_priod==0)
				{
					key_pressed = 1;
					tc = 10;
				}
				delay200us();
				if(tc)
				{
					tc--;
					if(!tc)
					{
						if(key_priod==0)
						{
							run_state = show_priod;
							key_pressed = 0;
						}
					else
						key_pressed = 0;	//判定抖动
					}
				}
				
				
				break;
			case show_priod:
				sampling_priod_display(sampling_priod);
				/*功能键检测*/
				if(key_pressed==0&&(key_priod==0||key_inc==0||key_dec==0||key_ok==0))
				{
					key_pressed = 1;
					tc = 10;
				}
				delay200us();
				if(tc)
				{
					tc--;
					if(!tc)
					{
						if(key_priod==0||key_inc==0||key_dec==0||key_ok==0)
						{
							if(key_priod==0)
							{
								run_state = show_upper_limit;
								key_pressed = 0;
							}
							else if(key_inc==0)
							{
								blink_position--;
								if(blink_position==0)
									blink_position=4;
								key_pressed=0;
							}
							else if(key_dec==0)
							{
								
								switch(blink_position)
								{
									case 1:
										sampling_priod+=10000;
										if(sampling_priod>max_sampling_priod)
											sampling_priod-=max_sampling_priod/10000*10000;	
									break;
									case 2:
										sampling_priod+=1000;
										if(sampling_priod>max_sampling_priod)
											sampling_priod-=max_sampling_priod/1000%10*1000;
										if(sampling_priod/1000%10==0)			//消除进位
											sampling_priod-=10000;
									break;
									case 3:
										sampling_priod+=100;
										if(sampling_priod>max_sampling_priod)
											sampling_priod-=max_sampling_priod/100%10*100;
										if(sampling_priod/100%10==0)
											sampling_priod-=1000;	
									break;	
									case 4:
										sampling_priod+=20;
										if(sampling_priod>max_sampling_priod)
											sampling_priod-=max_sampling_priod/10%10*10;
										if(sampling_priod/10%10==0)
											sampling_priod-=100;
									break;	
									default:
										break;
								}
								key_pressed=0;
							}
							else if(key_ok==0)
							{
								run_state=show_temperature;
								blink_position=4;
								key_pressed=0;
							}
							else
							{
								key_pressed = 0;	//判定抖动
							}
						}
						else
							key_pressed = 0;	//判定抖动
					}
				}
				
				break;
			case show_upper_limit:
				upper_limit_display(temperature_alarm_upper_limit);
				/*功能键检测*/
				if(key_pressed==0&&(key_priod==0||key_inc==0||key_dec==0||key_ok==0))
				{
					key_pressed = 1;
					tc = 10;
				}
				delay200us();
				if(tc)
				{
					tc--;
					if(!tc)
					{
						if(key_priod==0||key_inc==0||key_dec==0||key_ok==0)
						{
							if(key_priod==0)
							{
								run_state = show_priod;
								key_pressed = 0;
							}
							else if(key_inc==0)
							{
								blink_position--;
								if(blink_position==0)
									blink_position=4;
								key_pressed=0;
							}
							else if(key_dec==0)
							{
								
								switch(blink_position)
								{
									case 1:
										temperature_alarm_upper_limit+=100;
										if(temperature_alarm_upper_limit>max_temperature)
											temperature_alarm_upper_limit-=max_temperature/100*100;	
									break;
									case 2:
										temperature_alarm_upper_limit+=10;
										if(temperature_alarm_upper_limit>max_temperature)
											temperature_alarm_upper_limit-=((int)(max_temperature/10))%10*10;
										if(((int)(temperature_alarm_upper_limit/10))%10==0)			//消除进位
											temperature_alarm_upper_limit-=100;
									break;
									case 3:
										temperature_alarm_upper_limit+=1;
										if(temperature_alarm_upper_limit>max_temperature)
											temperature_alarm_upper_limit-=((int)(max_temperature))%10;
										if(((int)(temperature_alarm_upper_limit))%10==0)			
											temperature_alarm_upper_limit-=10;
									break;
									case 4:
										temperature_alarm_upper_limit+=0.5;
										if(temperature_alarm_upper_limit>max_temperature)
											temperature_alarm_upper_limit-=1;
										if(temperature_alarm_upper_limit-(int)(temperature_alarm_upper_limit)==0)			
											temperature_alarm_upper_limit-=1;
									break;	
									default:
										break;
								}
								key_pressed=0;
							}
							else if(key_ok==0)
							{
								run_state=show_temperature;
								blink_position=4;
								key_pressed=0;
							}
							else
							{
								key_pressed = 0;	//判定抖动
							}
						}
						else
							key_pressed = 0;	//判定抖动
					}
				}
				
				break;
		}
		
	}
	
}
//***********************************************************************
void temp_display(int temperature)
{
	unsigned char temp[] = {0x10,0,0,0,0x11,0x0c};
	int i = 0;
	for(i = 4;temperature;i--)//11.30
	{
		temp[i-1] = temperature%10;
		temperature/=10;
	}
	single_tube_display(0,seg_tab[temp[0]]);
	single_tube_display(1,seg_tab[temp[1]]);
	single_tube_display(2,seg_tab[temp[2]]&0x7f);	//与上小数点
	single_tube_display(3,seg_tab[temp[3]]);
	single_tube_display(4,seg_tab[temp[4]]);
	single_tube_display(5,seg_tab[temp[5]]);
	
}
//***********************************************************************
void sampling_priod_display(unsigned int sampling_priod)
{
	unsigned char priod[] = {0x12,0,0,0,0,0};
	unsigned char i = 5;
	static unsigned int blink_time = 0;
	static bit blink_flag = 1;
	if(blink_flag)
	{
		if(++blink_time==30)
			blink_flag = 0;
	}
	else
	{
		if(--blink_time==0)
			blink_flag = 1;
	}
	while(i)
	{
		priod[i] = sampling_priod%10;
		sampling_priod/=10;
		i--;								//11.26
	}
	/*闪烁调整 所以每位必须点亮*/
//	if(priod[1] == 0)	priod[1] = 0x10;	//空
//	if(priod[2] == 0)	priod[2] = 0x10;	//空
	single_tube_display(1,seg_tab[priod[0]]);
	switch(blink_position)
	{
		case 1:
			if(blink_flag)
				single_tube_display(2,seg_tab[priod[1]]);
			else
				single_tube_display(2,seg_tab[0x10]);//空
			single_tube_display(3,seg_tab[priod[2]]);
			single_tube_display(4,seg_tab[priod[3]]);
			single_tube_display(5,seg_tab[priod[4]]);
			single_tube_display(6,seg_tab[priod[5]]);
			break;
		case 2:
			if(blink_flag)
				single_tube_display(3,seg_tab[priod[2]]);
			else
				single_tube_display(3,seg_tab[0x10]);//空
			single_tube_display(2,seg_tab[priod[1]]);
			single_tube_display(4,seg_tab[priod[3]]);
			single_tube_display(5,seg_tab[priod[4]]);
			single_tube_display(6,seg_tab[priod[5]]);
			break;
		case 3:
			if(blink_flag)
				single_tube_display(4,seg_tab[priod[3]]);
			else
				single_tube_display(4,seg_tab[0x10]);//空
			single_tube_display(3,seg_tab[priod[2]]);
			single_tube_display(2,seg_tab[priod[1]]);
			single_tube_display(5,seg_tab[priod[4]]);
			single_tube_display(6,seg_tab[priod[5]]);
			break;
		case 4:
			if(blink_flag)
				single_tube_display(5,seg_tab[priod[4]]);
			else
				single_tube_display(5,seg_tab[0x10]);//空
			single_tube_display(3,seg_tab[priod[2]]);
			single_tube_display(4,seg_tab[priod[3]]);
			single_tube_display(2,seg_tab[priod[1]]);
			single_tube_display(6,seg_tab[priod[5]]);
			break;
		case 5:
			if(blink_flag)
				single_tube_display(6,seg_tab[priod[5]]);
			else
				single_tube_display(6,seg_tab[0x10]);//空
			single_tube_display(3,seg_tab[priod[2]]);
			single_tube_display(4,seg_tab[priod[3]]);
			single_tube_display(5,seg_tab[priod[4]]);
			single_tube_display(2,seg_tab[priod[1]]);
			break;
			
	}

}
//***********************************************************************
void upper_limit_display(float upper)
{
	unsigned char temperature[] = {0x0e,0x12,0x00,0x00,0x00,0x00};
	unsigned char i = 5;
	int upper_diplay = (int)(upper * 10);
	static unsigned int blink_time = 0;
	static bit blink_flag = 1;
	if(blink_flag)
	{
		if(++blink_time==30)
			blink_flag = 0;
	}
	else
	{
		if(--blink_time==0)
			blink_flag = 1;
	}
	while(upper_diplay)
	{
		temperature[i] = upper_diplay%10;
		upper_diplay/=10;
		i--;								//12.7
	}
	single_tube_display(0,seg_tab[temperature[0]]);
	single_tube_display(1,seg_tab[temperature[1]]);

	
	switch(blink_position)
	{
		case 1:
			if(blink_flag)
				single_tube_display(2,seg_tab[temperature[2]]);
			else
				single_tube_display(2,seg_tab[0x10]);//空
			single_tube_display(3,seg_tab[temperature[3]]);
			single_tube_display(4,seg_tab[temperature[4]]&0x7f);
			single_tube_display(5,seg_tab[temperature[5]]);
			break;
		case 2:
			if(blink_flag)
				single_tube_display(3,seg_tab[temperature[3]]);
			else
				single_tube_display(3,seg_tab[0x10]);//空
			single_tube_display(2,seg_tab[temperature[2]]);
			single_tube_display(4,seg_tab[temperature[4]]&0x7f);
			single_tube_display(5,seg_tab[temperature[5]]);
			break;
		case 3:
			if(blink_flag)
				single_tube_display(4,seg_tab[temperature[4]]&0x7f);
			else
				single_tube_display(4,seg_tab[0x10]&0x7f);//空
			single_tube_display(3,seg_tab[temperature[3]]);
			single_tube_display(2,seg_tab[temperature[2]]);
			single_tube_display(5,seg_tab[temperature[5]]);
			break;
		case 4:
			if(blink_flag)
				single_tube_display(5,seg_tab[temperature[5]]);
			else
				single_tube_display(5,seg_tab[0x10]);//空
			single_tube_display(3,seg_tab[temperature[3]]);
			single_tube_display(4,seg_tab[temperature[4]]&0x7f);
			single_tube_display(2,seg_tab[temperature[2]]);
			break;
	}
	single_tube_display(6,seg_tab[0x11]);
	single_tube_display(7,seg_tab[0x0c]);
}
//***********************************************************************
void timer0_init(void)	//20ms	12mhz
{
	TMOD &= 0xF0;		
	TMOD |= 0x01;		
	TL0 = 0xE0;	
	TH0 = 0xB1;
	TF0 = 0;		
	TR0 = 1;		
}
//***********************************************************************
void timer0(void) interrupt 1
{
	TL0 = 0xE0;	
	TH0 = 0xB1;
	if(run_state==show_temperature)
		temp_display(temp);
	count++;
}