#ifndef _SERVER_CTRL_H_
#define _SERVER_CTRL_H_

#include "common.h"
#include "include.h"

typedef struct
{
    float Set_Speed;            //Ԥ���ٶ�
    float Current_Speed;        //��ǰ�ٶ�
    float Set_Angle;            //Ԥ���Ƕ�
    float Current_Angle;        //��ǰ�Ƕ�  
    //float Current_AngleS;
    float Error;                //��ǰƫ��
    float Last_Error;           //�ϴ�ƫ��
    float Past_Error;
    float Kp;                   //����ϵ��
    //float Kpout;
    float Ki;                   //����ϵ��
    float Kd;                   //΢��ϵ��
    //float OUT;                  //��̬ƫ��
    float OUTPWM;               //������
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


#endif