#include "Motor_ctrl.h"
#include "Server_ctrl.h"

PID Motor;

uint32 viameter = 0;//路程计数器
uint8 viameter_on_flag = 0;//路程计数器开启标志位

#define MOTOR_OUTPWM_MAX 70
/*!
 *  @brief      电机初始化
 *  @param      
 *  @param      
 *  @since      
 *  @note       
 */
void Motor_Init()
{
    ftm_pwm_init(FTM0, FTM_CH2,17*1000,0,FTM0_CH2_PIN);
    ftm_pwm_init(FTM0, FTM_CH3,17*1000,0,FTM0_CH3_PIN);
    Motor.OUTPWM = 0;
}

void Motor_Run(float *duty)
{
    if(StopFlag1||StopFlag)(*duty) = 0;
    if(*duty>MOTOR_OUTPWM_MAX)(*duty) = MOTOR_OUTPWM_MAX;
    if(*duty<-MOTOR_OUTPWM_MAX)(*duty) = -MOTOR_OUTPWM_MAX;//输出限幅
    if(*duty > 0)
    {
        ftm_pwm_duty(FTM0,FTM_CH3,*duty);
        ftm_pwm_duty(FTM0,FTM_CH2,0);
    }
    else
    {
        ftm_pwm_duty(FTM0,FTM_CH3,0);
        ftm_pwm_duty(FTM0,FTM_CH2,-*duty);
    }
}

void Encoder_init()
{
    ftm_quad_init(FTM1,FTM_PS_1,FTM1_QDPHA_PIN,FTM1_QDPHB_PIN);
}

void Get_Speed()
{
    Motor.Current_Speed = ftm_quad_get(FTM1);          //获取FTM 正交解码 的脉冲数(负数表示反方向)
    ftm_quad_clean(FTM1);
    if(viameter_on_flag)viameter += Motor.Current_Speed;
    else viameter = 0;
}

void Speed_PID_Ctrl()
{
    float perror;
    float this_error = Motor.Set_Speed - Motor.Current_Speed;
    perror = this_error - Motor.Last_Error;
    Motor.OUTPWM += 0.01*(Motor.Kp*perror + this_error*Motor.Ki);
#ifdef MOTOR_PID_UP_DATA
    UP_Value[0] = (int32)Motor.Set_Speed;
    UP_Value[1] = (int32)Motor.Current_Speed;
    UP_Value[2] = (int32)this_error;
    UP_Value[3] = (int32)Motor.OUTPWM;
    UP_Value[4] = (int32)Motor.Last_Error;
    UP_Value[5] = (int32)perror;
    UP_Value[6] = 0;
    UP_Value[7] = 0;
#endif
    Motor.Last_Error = this_error;
    Motor_Run(&Motor.OUTPWM);
    
}