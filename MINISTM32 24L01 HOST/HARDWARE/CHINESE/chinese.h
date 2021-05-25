#ifndef __CHINESE_H
#define __CHINESE_H 
#include "sys.h"


struct typFNT_GB12 // 汉字字模数据结构体
{
u8 Index[2];// 汉字内码索引,汉字在内存中占两个字节
char Msk[24]; // 点阵码数据
};
struct typFNT_GB16 // 汉字字模数据结构体
{
u8 Index[2];// 汉字内码索引,汉字在内存中占两个字节
char Msk[32]; // 点阵码数据
};
struct typFNT_GB24 // 汉字字模数据结构体
{
u8 Index[2];// 汉字内码索引,汉字在内存中占两个字节
char Msk[72]; // 点阵码数据
};


void show_chinese(u16 x,u16 y,u8 size,u8 *p,u16 Word_Color,u16 Back_Color);
void show_chinese12(u16 x,u16 y,u8 size,u8 *p);
void show_chinese16(u16 x0,u16 y0,u8 size,u8 *p);
void show_chinese24(u16 x,u16 y,u8 size,u8 *p);
#endif