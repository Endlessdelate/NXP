#include <stdlib.h>
#include "menu.h"
#include "oled.h"
#include "iic.h"
#include "key.h"
#include "ADC.h"
#include "Server_ctrl.h"
#include "Motor_ctrl.h"
#include "Superware.h"

/********* �ڴ˴���Ӻ������� *********/

void Set_Parameter(float * value);      //���ò���
void Inner_Flash(uint8 flag);           //�ڲ�flash����
void Parameter_Manage(void);            //��������ͼ���

//void Show_Image(void);
void Show_AD(void);
void Show_Pid(void);
void Set_Pid(void);
void Show_Distance(void);
/***************��������***************/
void Empty(void); //�պ�������ɾ��

menu_operate menu_op = {0, 0, 0, 0, 0, 0, NULL};        //����˵������ṹ�����
const submenu menus[] =                                //�ڴ˴���Ӳ˵�,�˵�����5�������8��
{
    {1, "load\0", Parameter_Manage},                        //�ڲ˵������һ������Empty������Ӧ�Ĺ��ܺ���
    {2, "save\0", Parameter_Manage},
    {3, "set\0", Empty},
    {4, "message\0", Empty},
    {31, "duoji pid\0", Empty},
    {32, "motor pid\0", Empty},
    {33, "m_Speed\0", Set_Pid},

    {41, "pid\0", Show_Pid},
    {42, "ad\0", Show_AD},
    {43, "Distance\0", Show_Distance},
    {311, "d_kp\0", Set_Pid},
    {312, "d_ki\0", Set_Pid},
    {313, "d_kd\0", Set_Pid},
    {321, "m_kp\0", Set_Pid},
    {322, "m_ki\0", Set_Pid},
    {323, "m_kd\0", Set_Pid},
    
};

static void Get_Deep(void);              //ͳ�Ʋ˵�������
static void Get_Level_Num(void);         //�õ���ǰ����˵���
static void Get_Menu_Num(void);          //��ȡ�˵�����
static void Menu_Forward(void);          //�л��������
static void Menu_Back(void);             //�˻���һ������
static void Menu_Down(void);             //���·�ҳ
static void Menu_Up(void);               //���Ϸ�ҳ
static void Chose_Menu(void);            //�����л�
static void Show_Menu(void);             //��ʾ�˵�
static void Exe_Fuc(void);               //ִ�в˵�����

/**
  * @brief  �պ���
  * @param  None
  * @retval None
  */
void Empty(void)
{
    ;
}

/**
  * @brief  ��ò˵�������
  * @param  None
  * @retval None
  */
static void Get_Deep(void)
{
    uint16 temp = 0;
    uint8 i = 0;

    temp = menus[0].menu_level;
    for(i = 1; i < menu_op.menu_nums; i ++)
    {
        if(temp < menus[i].menu_level)
            temp = menus[i].menu_level;
    }
    i = 0;
    while(temp != 0)
    {
        temp /= 10;
        i ++;
    }
    menu_op.max_deep = i;
}

/**
  * @brief  ��õ�ǰ��˵���
  * @param  None
  * @retval None
  */
static void Get_Level_Num(void)
{
    uint16 temp = 0;
    uint8 i = 0;
    uint8 j = 0;

    for(i = 0; i < menu_op.menu_nums; i ++)
    {
        temp = menus[i].menu_level;
        if(temp / 10 == menu_op.current_location / 10)
            j ++;
    }
    menu_op.max_level = j;
}

/**
  * @brief  ��ȡ�˵�����
  * @param  None
  * @retval None
  */
static void Get_Menu_Num(void)
{
    menu_op.menu_nums = sizeof(menus) / sizeof(menus[0]);
}

/**
  * @brief  ��ǰ��һ��
  * @param  None
  * @retval None
  */
static void Menu_Forward(void)
{
    uint8 i = 0;
    uint8 j = 0;
    uint16 temp = 0;

    if(menu_op.current_location == 0)   //���������һ������
    {
        menu_op.current_location = 1;
    }
    else
    {
        menu_op.current_location *= 10;
        menu_op.current_location += 1;
        menu_op.now_deep ++;
    }

    for(i = 0; i < menu_op.menu_nums; i ++)         //��Ѱ������һ������
    {
        temp = menus[i].menu_level;
        if(temp == menu_op.current_location)
            j ++;
    }
    if(j == 0)                                      //���û����һ��������˻���һ��,��ֹ��������
    {
        menu_op.exe_flag = 1;                       //����ִ�б�־λ��һ
        menu_op.current_location /= 10;
        menu_op.now_deep --;
    }
}

/**
  * @brief  �����һ��
  * @param  None
  * @retval None
  */
static void Menu_Back(void)
{
    menu_op.current_location /= 10;
    menu_op.now_deep --;
    if(menu_op.now_deep < 0)
    {
        menu_op.current_location = 0;
        menu_op.now_deep = 0;
    }
}

/**
  * @brief  ���·�
  * @param  None
  * @retval None
  */
static void Menu_Down(void)
{
    Get_Level_Num();                    //��õ�ǰ�˵���

    if(menu_op.current_location != 0)   //��ֹ�׽������·�����
    {
        menu_op.current_location += 1;
        if(menu_op.current_location % 10 > menu_op.max_level)
            menu_op.current_location -= 1;
    }
}

/**
  * @brief  ���Ϸ�
  * @param  None
  * @retval None
  */
static void Menu_Up(void)
{
    Get_Level_Num();                    //��õ�ǰ�˵���

    if(menu_op.current_location != 0)   //��ֹ�׽������·�����
    {
        menu_op.current_location -= 1;
        if(menu_op.current_location % 10 == 0)
            menu_op.current_location += 1;
    }
}

/**
  * @brief  �������ƣ�ѡ��˵�����ִ��
  * @param  None
  * @retval None
  */
static void Chose_Menu(void)            //�����л�
{
    extern KEY key;

    switch (key.key_value)
        {
        case OK:
            break;
        case SET:
            Menu_Forward();
            break;
        case BACK:
            Menu_Back();
            break;
        case DOWN:
            Menu_Down();
            break;
        case UP:
            Menu_Up();
            break;
        case RIGHT:
            break;
        case LEFT:
            break;
        }
}

/**
  * @brief  ��ʾ�˵�
  * @param  None
  * @retval None
  */
static void Show_Menu(void)
{
    uint8 i = 0;
    uint8 j = 0;
    uint16 temp = 0;
    char buf[22] = {"\0"};

    OLED_CLS();
    for(i = 0; i < menu_op.menu_nums; i ++)                             //��ʾ�ַ���
    {
        temp = menus[i].menu_level;
        if(temp / 10 == menu_op.current_location / 10)
        {
            sprintf(buf, "%d.%s", j + 1,menus[i].str);
            OLED_P6x8Str(0, j, buf);
            j ++;
        }
    }
    OLED_P6x8Str(CURSOR_DIS, menu_op.current_location % 10 - 1, SHAPE);   //��ʾ���
}

/**
  * @brief  ִ�в˵����ܺ���
  * @param  None
  * @retval None
  */
static void Exe_Fuc(void)                               //ִ�в˵�����
{
    extern KEY key;
    uint8 i = 0;

    OLED_CLS();
    if(menu_op.exe_flag == 1)
    {
        menu_op.exe_flag = 0;
        for(i = 0; i < menu_op.menu_nums; i ++)         //��ʾ�ַ���
        {
            if((menus[i].menu_level == menu_op.current_location) && (menus[i].fuc != Empty))
            {
                menu_op.current_menu = &menus[i];
                key.key_flag = 1;                       //ʹ�õ�һ�α�ִ�У��ñ�־λ��chang_level�б������
                while(1)
                {
                     if(key.key_flag == 1)
                     {
                         key.key_flag = 0;
                         if(key.key_value == BACK)       //�˳�
                            break;
                         menus[i].fuc();                 //ֻ��ִ��һ�Σ���Ҫѭ��ִ�У�������ѭ��
                     }
                }
            }
        }
    }
}

/**
  * @brief  �˵���ʼ��
  * @param  None
  * @retval None
  */
void Menu_Init(void)
{
    extern uint8 house[];

    OLED_Show_Length(house);    //��ʾ��ͼƬ

    Get_Menu_Num();
    Get_Deep();
    Get_Level_Num();
    
    //Inner_Flash(2);           //�����Ĺ������ݱ��浽flash
}

/**
  * @brief  �˵��л�
  * @param  None
  * @retval None
  */
void Change_Level(void)
{
    extern KEY key;
    extern uint8 house[];

	if(key.key_flag == 1)   //key_flag���ж�����λ
    {
        key.key_flag = 0;
        Chose_Menu();
        Exe_Fuc();
        Show_Menu();
        if(menu_op.current_location == 0)
            OLED_Show_Length(house);    //��ʾ�׽���
    }
}

/********************* �ڴ˴���Ӻ���,ע�⺯��ֻ�ᱻִ��һ�Σ���Ҫѭ��ִ�м���ѭ�� **********************/

/**
  * @brief  ���ò���
  * @param  ����ָ��
  * @retval None
  */
void Set_Parameter(float *value)
{
    extern KEY key;
    char temp[20] = {"\0"};
    static uint8 count = 0;
    uint8 once_flag = 0;            //��֤С����ֻ������һ�Σ���ֹatof��������

    sprintf(temp, "%f", *value);    //��ֵ���ص�������ȥ
    while(1)
    {
        if(key.key_value == BACK)
        {
            key.key_flag = 1;       //��֤���������ѭ��
            *value = atof(temp);    //�˳�ǰ������������ַ�����Ϊ������
            break;
        }
        else if(key.key_flag == 1)
        {
            key.key_flag = 0;
            if(key.key_value >= 0 && key.key_value <= 9)
            {
                temp[count] = key.key_value + '0';       //��������ת��ΪASCLL��
                count ++;
                if(count == 10)
                    count = 9;
            }
            else if(key.key_value == ZERO)
            {
                temp[count] = '0';
                count ++;
                if(count == 10)
                    count = 9;
            }
            else if(key.key_value == DOT && once_flag != 1 && count != 0)   //�ж�С����,��֤��һ������С����
            {
                once_flag = 1;                                              //ʹС����ֻ������һ��
                temp[count] = '.';
                count ++;
                if(count == 10)
                    count = 9;
            }
        }
        disable_irq (PORTA_IRQn);
        OLED_P6x8Str(0, 0, temp);
        enable_irq (PORTA_IRQn);
    }
    count = 0;               //ע��Ҫ����
}

/**
  * @brief  �ڲ�flahs����
  *�Զ�����Ҫ��������ݣ����ȷ������ȫΪ 0XFF �������ʶ���쳣
  *���������ܷ������Ƕ��һ��������ֵ��ֵΪ0����
  * @param  1���棬2����
  * @retval None
  */
void Inner_Flash(uint8 flag)
{
    flash_data_t data;

    if(flag == 1)
    {
        data.sectornum_start    = FLASH_SECTOR_NUM - 6;             //��ʼ���� ,������6����������Ϊ�������
        data.sectornum_end    = FLASH_SECTOR_NUM - 4;               //��������

        data.data_addr      = &Motor;                          //���ݵĵ�ַ
        data.data_size      = sizeof(Motor);                   //���ݵĴ�С

        flash_data_init(&data);
        flash_data_save(&data);

        data.sectornum_start    = FLASH_SECTOR_NUM - 3;             //��ʼ���� ,������6����������Ϊ�������
        data.sectornum_end    = FLASH_SECTOR_NUM - 1;               //��������

        data.data_addr      = &Server;                          //���ݵĵ�ַ
        data.data_size      = sizeof(Server);                   //���ݵĴ�С

        flash_data_init(&data);
        flash_data_save(&data);
    }
    else if(flag == 2)
    {
        data.sectornum_start    = FLASH_SECTOR_NUM - 6;             //��ʼ���� ,������6����������Ϊ�������
        data.sectornum_end    = FLASH_SECTOR_NUM - 4;               //��������

        data.data_addr      = &Motor;                          //���ݵĵ�ַ
        data.data_size      = sizeof(Motor);                   //���ݵĴ�С

        flash_data_init(&data);
        flash_data_load(&data);

        data.sectornum_start    = FLASH_SECTOR_NUM - 3;             //��ʼ���� ,������6����������Ϊ�������
        data.sectornum_end    = FLASH_SECTOR_NUM - 1;               //��������

        data.data_addr      = &Server;                          //���ݵĵ�ַ
        data.data_size      = sizeof(Server);                   //���ݵĴ�С

        flash_data_init(&data);
        flash_data_load(&data);
    }
}
/**
  * @brief  �ڲ�flahs����,�����������ز���
  * @param  None
  * @retval None
  */
void Parameter_Manage(void)
{
    char buf[22] = {"\0"};
    if(!strcmp((char const *)menu_op.current_menu -> str,"load"))       //�Ƚϵ�ǰ�˵�
        Inner_Flash(2);
    else if(!strcmp((char const *)menu_op.current_menu -> str,"save"))
        Inner_Flash(1);

    sprintf(buf, "d_kp:%f  ", Server.Kp);
    OLED_P6x8Str(0, 0, buf);
    sprintf(buf, "d_ki:%f  ", Server.Ki);
    OLED_P6x8Str(0, 1, buf);
    sprintf(buf, "d_kd:%f  ", Server.Kd);
    OLED_P6x8Str(0, 2, buf);
    sprintf(buf, "m_kp:%f  ", Motor.Kp);
    OLED_P6x8Str(0, 3, buf);
    sprintf(buf, "m_ki:%f  ", Motor.Ki);
    OLED_P6x8Str(0, 4, buf);
    sprintf(buf, "m_kd:%f  ", Motor.Kd);
    OLED_P6x8Str(0, 5, buf);
    sprintf(buf, "m_Sd:%f  ", Motor.Set_Speed);
    OLED_P6x8Str(0, 6, buf);
}

/**
  * @brief  ��ʾ����ͷͼ��
  * @param  None
  * @retval None
  */
void Show_Image(void)
{
    extern uint8 imgbuff[CAMERA_SIZE];
    extern KEY key;

    while(key.key_value != BACK)      //ѭ��ִ�У������˳�
    {
        camera_get_img();
        OLED_Show_Cross(imgbuff, CAMERA_W, CAMERA_H);
    }
}

/**
  * @brief  ��ʾADת��ֵ
  * @param  None
  * @retval None
  */
void Show_AD(void)
{
  char buf[22] = {"\0"};
  int Angle=0,AngleS=0;
  extern KEY key;
  //disable_irq(PIT0_IRQn);
  while(key.key_value != BACK)
  {
    Read_ADC();
    //Angle=(int)(100*((R_AD+RM_AD-LM_AD-L_AD)/(float)(L_AD+RM_AD+R_AD+LM_AD+0.9*M_AD))); 
    //AngleS= (int)(100*(RS_AD-LS_AD)/(float)(RS_AD+LS_AD+MS_AD*0.8));
    sprintf(buf, "L:%d    ", L_AD);
    
    
    disable_irq (PORTA_IRQn);
    
    
    OLED_P6x8Str(0, 0, buf);
  
    sprintf(buf, "R:%d    ", R_AD);
    OLED_P6x8Str(0, 1, buf);
  
    sprintf(buf, "LM:%d    ", LM_AD);
    OLED_P6x8Str(60, 0, buf);
   
    sprintf(buf, "RM:%d    ", RM_AD);
    OLED_P6x8Str(60, 1, buf);
  
    sprintf(buf, "M:%d    ", M_AD);
    OLED_P6x8Str(0, 2, buf);
  
    sprintf(buf, "MS:%d    ", MS_AD);
    OLED_P6x8Str(0, 3, buf);
    
    sprintf(buf, "RS:%d    ", RS_AD);
    OLED_P6x8Str(60, 2, buf);
    
    sprintf(buf, "LS:%d    ", LS_AD);
    OLED_P6x8Str(60, 3, buf);
    
    sprintf(buf, "Angle:%d    ", Angle);
    OLED_P6x8Str(0, 4, buf);
    
    sprintf(buf, "AngleS:%d    ", AngleS);
    OLED_P6x8Str(0, 5, buf);
    
    sprintf(buf, "AngleS+Angle:%d    ", (AngleS+Angle));
    OLED_P6x8Str(0, 6, buf);
    enable_irq (PORTA_IRQn);
    DELAY_MS(7);
  }
  //enable_irq(PIT0_IRQn); 
}

/**
  * @brief  ��ʾPID����
  * @param  None
  * @retval None
  */
void Show_Pid(void)
{
    char buf[22] = {"\0"};

    sprintf(buf, "d_kp: %f", Server.Kp);
    OLED_P6x8Str(0, 0, buf);
    sprintf(buf, "d_ki: %f", Server.Ki);
    OLED_P6x8Str(0, 1, buf);
    sprintf(buf, "d_ki: %f", Server.Kd);
    OLED_P6x8Str(0, 2, buf);
    sprintf(buf, "m_kp: %f", Motor.Kp);
    OLED_P6x8Str(0, 3, buf);
    sprintf(buf, "m_ki: %f", Motor.Ki);
    OLED_P6x8Str(0, 4, buf);
    sprintf(buf, "m_kd: %f", Motor.Kd);
    OLED_P6x8Str(0, 5, buf);
    sprintf(buf, "Set_Speed: %f", Motor.Set_Speed);
    OLED_P6x8Str(0, 6, buf);
}

/**
  * @brief  ����PID����
  * @param  None
  * @retval None
  */
void Set_Pid(void)
{
    disable_irq(PIT0_IRQn);
    if(!strcmp((char const *)menu_op.current_menu -> str,"d_kp"))       //�Ƚϵ�ǰ�˵�
        Set_Parameter(&Server.Kp);
    else if(!strcmp((char const *)menu_op.current_menu -> str,"d_ki"))
        Set_Parameter(&Server.Ki);
    else if(!strcmp((char const *)menu_op.current_menu -> str,"d_kd"))
        Set_Parameter(&Server.Kd);
    else if(!strcmp((char const *)menu_op.current_menu -> str,"m_kp"))
        Set_Parameter(&Motor.Kp);
    else if(!strcmp((char const *)menu_op.current_menu -> str,"m_ki"))
        Set_Parameter(&Motor.Ki);
    else if(!strcmp((char const *)menu_op.current_menu -> str,"m_kd"))
        Set_Parameter(&Motor.Kd);
    else if(!strcmp((char const *)menu_op.current_menu -> str,"m_Speed"))
        Set_Parameter(&Motor.Set_Speed);
    enable_irq(PIT0_IRQn);
}
/*****��ʾ����������*****/
void Show_Distance(void)
{
  char buf1[22] = {"\0"};
  sprintf(buf1, "Dis:%f mm", distance);
  OLED_P6x8Str(0, 0, buf1);
  
 
}