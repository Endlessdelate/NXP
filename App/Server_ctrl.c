/*！
 * @file       Server_ctrl.c
 * @brief      猎户座舵机PID控制
 * @author     MAZY
 * @version    v1.0
 * @date       2019-06-12
*/

/*
* When I wrote this, only God and I understood what I was doing
* Now, God only knows
* 写这段代码的时候，只有上帝和我知道它是干嘛的
* 现在，只有上帝知道
*/

//2019.6.18
#include "Server_ctrl.h"
#include "Motor_ctrl.h"
#define SERVER_MIDDLE 7725
#define SERVER_LIMIT 1700

uint8 lock_status_flag = 0;//运行状态锁定位
   
   
enum Run_Status//运行状态枚举
{
    Normal,//正常
    Lost_track,//丢线
    Right_angle,//直角
    Annulus,//圆环
    Roadblock,//路障
    Stop//停车
}Run_status;

PID Server;

void PID_Init(void)
{
    Server.Kp = 18;
    Server.Ki = 0;
    Server.Kd = 40;
    
    Motor.Kp = 50;
    Motor.Ki = 5;
    Motor.Kd = 0;
    Motor.Set_Speed = 200;
}

void Server_Init()
{
    ftm_pwm_init(FTM2,FTM_CH0,50,SERVER_MIDDLE,FTM2_CH0_PIN);
}

void Server_Run(float duty)
{
#ifdef SERVER_DEBUG
    float text_duty = 0;
    if(key_get(KEY_1) == KEY_DOWN)
    {
        if(key_get(KEY_2) == KEY_DOWN)text_duty = SERVER_MIDDLE;
        else text_duty = SERVER_MIDDLE+SERVER_LIMIT;//右
    }
    else text_duty = SERVER_MIDDLE-SERVER_LIMIT;//左

    ftm_pwm_duty(FTM2, FTM_CH0,text_duty/1000.0);
#endif
#ifndef SERVER_DEBUG
    if(duty > SERVER_MIDDLE+SERVER_LIMIT)duty = SERVER_MIDDLE+SERVER_LIMIT;
    if(duty < SERVER_MIDDLE-SERVER_LIMIT)duty = SERVER_MIDDLE-SERVER_LIMIT;
    ftm_pwm_duty(FTM2, FTM_CH0,duty/1000.0);
#endif
}

void Server_PID_Ctrl()
{
    float derror;
    Get_Direction_Error();
    derror = Server.Error - Server.Last_Error;
    Server.OUTPWM = SERVER_MIDDLE - (Server.Kp*Server.Error + Server.Kd*derror);
#ifdef SERVER_PID_UP_DATA
    UP_Value[0] = (int32)error;
    UP_Value[1] = (int32)Server.Last_Error;
    UP_Value[2] = (int32)Server.OUTPWM;
#endif 
    Server.Last_Error = Server.Error;
    Server_Run(Server.OUTPWM);
}
enum Run_Status Get_Status()
{
    if((L_AD<=5)&&(R_AD<=5))return Lost_track;
    if((M_AD>650)&&(abs(L_AD-R_AD)>200))return Annulus;//中间值特别大，两边差很大
    if((abs(LM_AD-RM_AD)>300)&&(L_AD<100&&R_AD<100))return Right_angle;//斜电感差值大，横电感差值小
    return Normal;
}
void Get_Direction_Error()
{
    if(!lock_status_flag)Run_status = Get_Status();//若没锁定，则更新状态
    switch(Run_status)
    {
        case Normal:Normal_Run();break;
        case Annulus:Run_In_Annulus();break;
        case Right_angle:Run_In_Right_angle();break;
        default:Normal_Run();
    }   
}

void Normal_Run()  //正常
{
    float error,xerror;
    lock_status_flag = 0;
    error  = 70.0*(L_AD-R_AD)/(float)(L_AD+R_AD+M_AD);
    xerror = 30.0*(LM_AD-RM_AD)/(float)(LM_AD+RM_AD+M_AD);
    Server.Error = error + xerror;
}

void Run_In_Annulus()   //圆环
{
    static uint8 annulus_direction;//环方向
    if(!lock_status_flag)//第一次进入，
    {
        lock_status_flag = 1;//锁定圆环状态
        viameter_on_flag = 1;//开启路程计数器
        beep_on = 3;//提示音
        if(L_AD>R_AD)//判断环方向
        {
            annulus_direction = 1;//左
        }
        else
        {
            annulus_direction = 0;//右
        }
    }
    if(viameter < 10000)//路程不到，固定偏差走
    {
      led (LED3,LED_ON);
        //if(annulus_direction)Server.Error = 100.0*(RM_AD*0.9-LM_AD*1.5)/(float)(LM_AD*1.5+RM_AD*0.9);  //L
        //else                 Server.Error = 100.0*(RM_AD*1.5-LM_AD*0.9)/(float)(LM_AD*0.9+RM_AD*1.5);  //R
		if(annulus_direction)Server.Error = 100.0*(LM_AD-R_AD)/(float)(LM_AD+R_AD) - 0.5*(M_AD);
        else                 Server.Error = 100.0*(LM_AD-R_AD)/(float)(LM_AD+R_AD) + 0.5*(M_AD);
    }
    else//路程到
    {
        lock_status_flag = 0;//解除圆环状态
        viameter_on_flag = 0;//关闭路程计数器
        led (LED3,LED_OFF);
    }
}

void Run_In_Right_angle()//直角模式
{
    static uint8 angle_direction;//直角方向
   // float error,xerror;
    if(!lock_status_flag)
    {
       lock_status_flag = 1;//锁定直角状态
       viameter_on_flag = 1;//开启路程计数器
       if(LM_AD > RM_AD)//判断直角方向
        {
            angle_direction = 1;//左
        }
        else
        {
            angle_direction = 0;//右
        }
		beep_on = 4;//提示音
    }
    if(viameter < 10000)//路程不到，固定偏差走
    {
      if(angle_direction)Server.Error=1600;
      else Server.Error=-1600;    
    }
    else
    {
      lock_status_flag = 0;//解锁状态
      viameter_on_flag = 0;//关闭路程计数器
    }
}