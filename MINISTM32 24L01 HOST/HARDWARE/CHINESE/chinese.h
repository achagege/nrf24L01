#ifndef __CHINESE_H
#define __CHINESE_H 
#include "sys.h"


struct typFNT_GB12 // ������ģ���ݽṹ��
{
u8 Index[2];// ������������,�������ڴ���ռ�����ֽ�
char Msk[24]; // ����������
};
struct typFNT_GB16 // ������ģ���ݽṹ��
{
u8 Index[2];// ������������,�������ڴ���ռ�����ֽ�
char Msk[32]; // ����������
};
struct typFNT_GB24 // ������ģ���ݽṹ��
{
u8 Index[2];// ������������,�������ڴ���ռ�����ֽ�
char Msk[72]; // ����������
};


void show_chinese(u16 x,u16 y,u8 size,u8 *p,u16 Word_Color,u16 Back_Color);
void show_chinese12(u16 x,u16 y,u8 size,u8 *p);
void show_chinese16(u16 x0,u16 y0,u8 size,u8 *p);
void show_chinese24(u16 x,u16 y,u8 size,u8 *p);
#endif