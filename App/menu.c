#include <stdlib.h>
#include "menu.h"
#include "oled.h"
#include "iic.h"
#include "key.h"
#include "ADC.h"
#include "Server_ctrl.h"
#include "Motor_ctrl.h"
#include "Superware.h"

/********* 在此处添加函数声明 *********/

void Set_Parameter(float * value);      //设置参数
void Inner_Flash(uint8 flag);           //内部flash调用
void Parameter_Manage(void);            //参数保存和加载

//void Show_Image(void);
void Show_AD(void);
void Show_Pid(void);
void Set_Pid(void);
void Show_Distance(void);
/***************函数声明***************/
void Empty(void); //空函数不可删除

menu_operate menu_op = {0, 0, 0, 0, 0, 0, NULL};        //定义菜单操作结构体变量
const submenu menus[] =                                //在此处添加菜单,菜单最深5级，最多8阶
{
    {1, "load\0", Parameter_Manage},                        //在菜单的最后一级，把Empty换成相应的功能函数
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

static void Get_Deep(void);              //统计菜单最大深度
static void Get_Level_Num(void);         //得到当前层面菜单数
static void Get_Menu_Num(void);          //获取菜单总数
static void Menu_Forward(void);          //切换狭义界面
static void Menu_Back(void);             //退回上一个界面
static void Menu_Down(void);             //向下翻页
static void Menu_Up(void);               //向上翻页
static void Chose_Menu(void);            //界面切换
static void Show_Menu(void);             //显示菜单
static void Exe_Fuc(void);               //执行菜单功能

/**
  * @brief  空函数
  * @param  None
  * @retval None
  */
void Empty(void)
{
    ;
}

/**
  * @brief  获得菜单最大深度
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
  * @brief  获得当前层菜单数
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
  * @brief  获取菜单总数
  * @param  None
  * @retval None
  */
static void Get_Menu_Num(void)
{
    menu_op.menu_nums = sizeof(menus) / sizeof(menus[0]);
}

/**
  * @brief  向前进一级
  * @param  None
  * @retval None
  */
static void Menu_Forward(void)
{
    uint8 i = 0;
    uint8 j = 0;
    uint16 temp = 0;

    if(menu_op.current_location == 0)   //单独处理第一个界面
    {
        menu_op.current_location = 1;
    }
    else
    {
        menu_op.current_location *= 10;
        menu_op.current_location += 1;
        menu_op.now_deep ++;
    }

    for(i = 0; i < menu_op.menu_nums; i ++)         //搜寻有无下一个界面
    {
        temp = menus[i].menu_level;
        if(temp == menu_op.current_location)
            j ++;
    }
    if(j == 0)                                      //如果没有下一个界面就退回上一级,防止超过界限
    {
        menu_op.exe_flag = 1;                       //函数执行标志位置一
        menu_op.current_location /= 10;
        menu_op.now_deep --;
    }
}

/**
  * @brief  向后退一级
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
  * @brief  向下翻
  * @param  None
  * @retval None
  */
static void Menu_Down(void)
{
    Get_Level_Num();                    //获得当前菜单数

    if(menu_op.current_location != 0)   //防止首界面上下翻超界
    {
        menu_op.current_location += 1;
        if(menu_op.current_location % 10 > menu_op.max_level)
            menu_op.current_location -= 1;
    }
}

/**
  * @brief  向上翻
  * @param  None
  * @retval None
  */
static void Menu_Up(void)
{
    Get_Level_Num();                    //获得当前菜单数

    if(menu_op.current_location != 0)   //防止首界面上下翻超界
    {
        menu_op.current_location -= 1;
        if(menu_op.current_location % 10 == 0)
            menu_op.current_location += 1;
    }
}

/**
  * @brief  按键控制，选择菜单功能执行
  * @param  None
  * @retval None
  */
static void Chose_Menu(void)            //界面切换
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
  * @brief  显示菜单
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
    for(i = 0; i < menu_op.menu_nums; i ++)                             //显示字符串
    {
        temp = menus[i].menu_level;
        if(temp / 10 == menu_op.current_location / 10)
        {
            sprintf(buf, "%d.%s", j + 1,menus[i].str);
            OLED_P6x8Str(0, j, buf);
            j ++;
        }
    }
    OLED_P6x8Str(CURSOR_DIS, menu_op.current_location % 10 - 1, SHAPE);   //显示光标
}

/**
  * @brief  执行菜单功能函数
  * @param  None
  * @retval None
  */
static void Exe_Fuc(void)                               //执行菜单功能
{
    extern KEY key;
    uint8 i = 0;

    OLED_CLS();
    if(menu_op.exe_flag == 1)
    {
        menu_op.exe_flag = 0;
        for(i = 0; i < menu_op.menu_nums; i ++)         //显示字符串
        {
            if((menus[i].menu_level == menu_op.current_location) && (menus[i].fuc != Empty))
            {
                menu_op.current_menu = &menus[i];
                key.key_flag = 1;                       //使得第一次被执行，该标志位在chang_level中被清零过
                while(1)
                {
                     if(key.key_flag == 1)
                     {
                         key.key_flag = 0;
                         if(key.key_value == BACK)       //退出
                            break;
                         menus[i].fuc();                 //只被执行一次，若要循环执行，加上死循环
                     }
                }
            }
        }
    }
}

/**
  * @brief  菜单初始化
  * @param  None
  * @retval None
  */
void Menu_Init(void)
{
    extern uint8 house[];

    OLED_Show_Length(house);    //显示马图片

    Get_Menu_Num();
    Get_Deep();
    Get_Level_Num();
    
    //Inner_Flash(2);           //将更改过的数据保存到flash
}

/**
  * @brief  菜单切换
  * @param  None
  * @retval None
  */
void Change_Level(void)
{
    extern KEY key;
    extern uint8 house[];

	if(key.key_flag == 1)   //key_flag在中断里置位
    {
        key.key_flag = 0;
        Chose_Menu();
        Exe_Fuc();
        Show_Menu();
        if(menu_op.current_location == 0)
            OLED_Show_Length(house);    //显示首界面
    }
}

/********************* 在此处添加函数,注意函数只会被执行一次，若要循环执行加死循环 **********************/

/**
  * @brief  设置参数
  * @param  参数指针
  * @retval None
  */
void Set_Parameter(float *value)
{
    extern KEY key;
    char temp[20] = {"\0"};
    static uint8 count = 0;
    uint8 once_flag = 0;            //保证小数点只会输入一次，防止atof函数出错

    sprintf(temp, "%f", *value);    //把值加载到缓存中去
    while(1)
    {
        if(key.key_value == BACK)
        {
            key.key_flag = 1;       //保证能跳出外层循环
            *value = atof(temp);    //退出前保存参数，把字符串改为浮点数
            break;
        }
        else if(key.key_flag == 1)
        {
            key.key_flag = 0;
            if(key.key_value >= 0 && key.key_value <= 9)
            {
                temp[count] = key.key_value + '0';       //输入数字转换为ASCLL码
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
            else if(key.key_value == DOT && once_flag != 1 && count != 0)   //判断小数点,保证第一个不是小数点
            {
                once_flag = 1;                                              //使小数点只能输入一次
                temp[count] = '.';
                count ++;
                if(count == 10)
                    count = 9;
            }
        }
        DisableInterrupts;
        OLED_P6x8Str(0, 0, temp);
        EnableInterrupts;
    }
    count = 0;               //注意要清零
}

/**
  * @brief  内部flahs调用
  *自定义需要保存的数据，务必确保不能全为 0XFF ，否则会识别异常
  *如果有这可能发生，那多加一个变量，值赋值为0即可
  * @param  1保存，2加载
  * @retval None
  */
void Inner_Flash(uint8 flag)
{
    flash_data_t data;

    if(flag == 1)
    {
        data.sectornum_start    = FLASH_SECTOR_NUM - 6;             //起始扇区 ,用最后的6个扇区来作为保存参数
        data.sectornum_end    = FLASH_SECTOR_NUM - 4;               //结束扇区

        data.data_addr      = &Motor;                          //数据的地址
        data.data_size      = sizeof(Motor);                   //数据的大小

        flash_data_init(&data);
        flash_data_save(&data);

        data.sectornum_start    = FLASH_SECTOR_NUM - 3;             //起始扇区 ,用最后的6个扇区来作为保存参数
        data.sectornum_end    = FLASH_SECTOR_NUM - 1;               //结束扇区

        data.data_addr      = &Server;                          //数据的地址
        data.data_size      = sizeof(Server);                   //数据的大小

        flash_data_init(&data);
        flash_data_save(&data);
    }
    else if(flag == 2)
    {
        data.sectornum_start    = FLASH_SECTOR_NUM - 6;             //起始扇区 ,用最后的6个扇区来作为保存参数
        data.sectornum_end    = FLASH_SECTOR_NUM - 4;               //结束扇区

        data.data_addr      = &Motor;                          //数据的地址
        data.data_size      = sizeof(Motor);                   //数据的大小

        flash_data_init(&data);
        flash_data_load(&data);

        data.sectornum_start    = FLASH_SECTOR_NUM - 3;             //起始扇区 ,用最后的6个扇区来作为保存参数
        data.sectornum_end    = FLASH_SECTOR_NUM - 1;               //结束扇区

        data.data_addr      = &Server;                          //数据的地址
        data.data_size      = sizeof(Server);                   //数据的大小

        flash_data_init(&data);
        flash_data_load(&data);
    }
}
/**
  * @brief  内部flahs调用,保存参数或加载参数
  * @param  None
  * @retval None
  */
void Parameter_Manage(void)
{
    char buf[22] = {"\0"};
    if(!strcmp((char const *)menu_op.current_menu -> str,"load"))       //比较当前菜单
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
  * @brief  显示摄像头图像
  * @param  None
  * @retval None
  */
void Show_Image(void)
{
    extern uint8 imgbuff[CAMERA_SIZE];
    extern KEY key;

    while(key.key_value != BACK)      //循环执行，除非退出
    {
        camera_get_img();
        OLED_Show_Cross(imgbuff, CAMERA_W, CAMERA_H);
    }
}

/**
  * @brief  显示AD转换值
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
  * @brief  显示PID参数
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
  * @brief  设置PID参数
  * @param  None
  * @retval None
  */
void Set_Pid(void)
{
    disable_irq(PIT0_IRQn);
    if(!strcmp((char const *)menu_op.current_menu -> str,"d_kp"))       //比较当前菜单
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
/*****显示超声波距离*****/
void Show_Distance(void)
{
  char buf1[22] = {"\0"};
  while(key.key_value != BACK)
  {
    sprintf(buf1, "Dis:%f mm", distance/10.0);
    OLED_P6x8Str(0, 0, buf1);
    Superware_Get_Distance();
  }
}