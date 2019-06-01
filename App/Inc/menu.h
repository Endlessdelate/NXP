#ifndef __MENU_H
#define __MENU_H

#include "common.h"
#include "include.h"

#define BACK            12           //����ֵ���壬������ȡֵ�Ͱ����Ŷ�Ӧ
#define SET             10
#define OK              11
#define UP              2
#define DOWN            8
#define LEFT            4
#define RIGHT           6
#define DOT             10          //С����
#define ZERO            11          //��

#define CURSOR_DIS      110         //����������
#define SHAPE           "<--"         //�����״


typedef struct
{
    uint16     menu_level;          //�˵��㼶
    uint8      *str;                //�洢�ַ���
    void       (*fuc)(void);        //�ü�����
}submenu;

typedef struct
{
    uint8               max_level;          //��ǰ������������
    uint8               max_deep;           //������
    uint8               menu_nums;          //�˵������
    int8                now_deep;           //��ǰ���
    uint8               exe_flag;           //ִ�к�����־λ
    uint16              current_location;   //��ǰλ��
    const submenu*      current_menu;      //��ǰ�˵�ָ��
}menu_operate;

void Menu_Init(void);
void Change_Level(void);

#endif
