#ifndef _SERVER_CTRL_H_
#define _SERVER_CTRL_H_

#include "common.h"
#include "include.h"

typedef struct
{
    float Set_Speed;            //预定速度
    float Current_Speed;        //当前速度
    float Set_Angle;            //预定角度
    float Current_Angle;        //当前角度  
    //float Current_AngleS;
    float Error;                //当前偏差
    float Last_Error;           //上次偏差
    float Past_Error;
    float Kp;                   //比例系数
    //float Kpout;
    float Ki;                   //积分系数
    float Kd;                   //微分系数
    //float OUT;                  //稳态偏零
    float OUTPWM;               //输出结果
    //float speed_out;
    
}PID;

extern PID Server;

void PID_Init(void);
void Server_Init();
void Server_Run(float duty);
void Server_PID_Ctrl();
void Get_Direction_Error();
void Normal_Run();
void Run_In_Annulus();
void Run_In_Right_angle();


#endif