#ifndef OLED_H
#define OLED_H

#include "common.h"
#include "include.h"

void OLED_Init(void);                                                           // OLED����ʼ�����򣬴˺���Ӧ�ڲ�����Ļ֮ǰ���ȵ���
void OLED_IIC_Init(void);
void OLED_WrDat(unsigned char dat);                                            // ��OLED��д����
void OLED_WrCmd(unsigned char cmd);                                            // ��OLED��д����
void OLED_Set_Pos(unsigned char x, unsigned char y);                          // ������ʾ����
void OLED_Fill(unsigned char bmp_dat);                                         // ȫ����ʾ(��ʾBMPͼƬʱ�Ż��õ��˹���)
void OLED_CLS(void);                                                            // ��λ/����
void OLED_P6x8Str(unsigned char x, unsigned char y, char ch[]);               // 6x8������������ʾASCII�����С���У���̫����
void OLED_P8x16Str(unsigned char x, unsigned char y, char ch[]);              // 8x16������������ʾASCII�룬�ǳ�����
void OLED_P16x16Ch(unsigned char x, unsigned char y, unsigned char N);        // 16x16������������ʾ���ֵ���С���У������ø������塢�Ӵ֡���б���»��ߵ�
void OLED_nurbem(uint8 x,uint8 y,uint8 a,uint8 b);
void OLED_nurbem_int(uint8 x,uint8 y,uint16 a);

void OLED_Show_Length(uint8 *table);                                            //��ʾ�����ͼƬ
void OLED_Show_Cross(uint8 *table, uint8 wide, uint8 high);                     //��ʾ�����ͼƬ
void OLED_Show_Uncompress(uint8 *Table,uint16 usRowNum, uint16 usColumnNum);    //��ʾ��ѹ���ͼƬ

#define	Brightness	0xCF
#define X_WIDTH 	128
#define Y_WIDTH 	64
#define LED_IMAGE_WHITE       0x01
#define LED_IMAGE_BLACK       0

#define SDAIN    gpio_get(IMU_SDA)

#endif