/*ds18b20驱动头文件
**@CTBU^Shy^
**$version:v1.0
**#chinese_coding_standard:GB2312
*/
#ifndef __DS18B20_H__
#define __DS18B20_H__

void delay10us(unsigned char count);
bit ds18b20_init(void);
void ds18b20_write_byte(unsigned char dat);
unsigned char ds18b20_read_byte(void);
void ds18b20_precision_set(unsigned char level);
int ds18b20_read_temperature(void);
#endif