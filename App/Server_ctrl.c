/*��
 * @file       Server_ctrl.c
 * @brief      �Ի������PID����
 * @author     MAZY
 * @version    v1.0
 * @date       2019-06-12
 */
#include "Server_ctrl.h"
#include "Motor_ctrl.h"
#define SERVER_MIDDLE 7725
#define SERVER_LIMIT 1700

uint8 lock_status_flag = 0;//����״̬����λ
   
   
enum Run_Status//����״̬ö��
{
    Normal,//����
    Lost_track,//����
    Annulus,//Բ��
    roadblock,//·��
    Stop//ͣ��
}Run_status;

PID Server;

void PID_Init(void)
{
    Server.Kp = 18;
    Server.Ki = 0;
    Server.Kd = 20;
    
    Motor.Kp = 50;
    Motor.Ki = 5;
    Motor.Kd = 0;
    Motor.Set_Speed = 100;
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
        else text_duty = SERVER_MIDDLE+SERVER_LIMIT;//��
    }
    else text_duty = SERVER_MIDDLE-SERVER_LIMIT;//��

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
    if(M_AD>800)return Annulus;
    return Normal;
}
void Get_Direction_Error()
{
    if(!lock_status_flag)Run_status = Get_Status();//��û�����������״̬
    switch(Run_status)
    {
        case Normal:Normal_Run();break;
        case Annulus:Run_In_Annulus();break;
        default:Normal_Run();
    }   
}

void Normal_Run()
{
    float error,xerror,serror;
    lock_status_flag = 0;
    error  = 100.0*(L_AD-R_AD)/(float)(L_AD+R_AD+M_AD);
    xerror = 100.0*(LM_AD-RM_AD)/(float)(LM_AD+RM_AD+M_AD);
    serror = 100.0*(LS_AD-RS_AD)/(float)(LS_AD+RS_AD+M_AD);
    Server.Error = 0.7*error + 0.3*xerror;//+ serror;
}

void Run_In_Annulus()
{
    static uint8 annulus_direction;//������
    if(!lock_status_flag)//��һ�ν��룬
    {
        lock_status_flag = 1;//����Բ��״̬
        viameter_on_flag = 1;//����·�̼�����
        beep_on = 3;//��ʾ��
        if(LM_AD > RM_AD)//�жϻ�����
        {
            //
            annulus_direction = 1;//��
        }
        else
        {
            //
            annulus_direction = 0;//��
        }
    }
    if(viameter < 9000)//·�̲������̶�ƫ����
    {
        if(annulus_direction)Server.Error = 100.0*(L_AD-R_AD)/(float)(L_AD+R_AD) - 0.5*(LM_AD+RM_AD);
        else                 Server.Error = 100.0*(L_AD-R_AD)/(float)(L_AD+R_AD) + 0.5*(LM_AD+RM_AD);
    }
    else//·�̵�
    {
        lock_status_flag = 0;//���Բ��״̬
        viameter_on_flag = 0;//�ر�·�̼�����
    }
}