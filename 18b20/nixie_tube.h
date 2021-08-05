/*8段共阳极数码管驱动头文件-动态
**@CTBU^Shy^
**$version:v1.0
**#chinese_coding_standard:GB2312
*/
#ifndef __NIXIE_TUBE_H__
#define __NIXIE_TUBE_H__


void single_tube_display(unsigned char position,unsigned char seg_dat);
void delay200us(void);
#endif