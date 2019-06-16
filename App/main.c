/*��
 * @file       main.c
 * @brief      �Ի���������
 * @author     MAZY
 * @version    v1.0
 * @date       2019-03-26
 */

#include "common.h"
#include "include.h"
#include  "define.h"
#include "Server_ctrl.h"
#include "Motor_ctrl.h"
   
uint8 StopFlag = 0,StopFlag1 = 0;
uint8 TimeCount1 = 0,TimeCount2 = 0;
uint8 mode = 0x00;

int32 UP_Value[8];

void PIT0_IRQHandler(void);    //
void PIT1_IRQHandler(void);    //��������ж�
void PORTD_IRQHandler(void);   //PORTC�˿��жϷ�����
void key_Stop(void);           //����ͣ��


/*!
 *  @brief      ϵͳ��ʼ������
 *  @since      v1.0
 *  @note       
 */
void System_Init()
{
                              
    led_init(LED0);
    led_init(LED1);
    led_init(LED2);
    led_init(LED3);
    
    
    
    ADC_Init();
    OLED_Init();          //OLED��ʼ��
    Key_Init();
    Menu_Init();          //�˵���ʼ��
    PID_Init();
    Encoder_init();
    Motor_Init();
    Server_Init();
    Beep_Init();
    Superware_Init();
    AD_Stop_Init();
    
    pit_init_ms(PIT0,2);                                     //��ʼ��PIT0����ʱʱ��Ϊ�� 2ms  �����������  
    set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);       //����PIT0���жϷ�����Ϊ PIT0_IRQHandler    
                                       //ʹ��PIT0�ж�  
        
    pit_init_ms(PIT1,5);                                    //��ʼ��PIT1����ʱʱ��Ϊ�� 5ms   �ٶȿ�������
    set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler);      //����PIT1���жϷ�����Ϊ PIT1_IRQHandler    
                                      //ʹ��PIT1�ж�  
        
    port_init(PTD4, ALT1 | IRQ_RISING | PULLDOWN );          //��ʼ�� PTD4 �ⲿ�жϹܽ�  ң��ͣ��
    set_vector_handler(PORTD_VECTORn ,PORTD_IRQHandler);    //����PORTD���жϷ�����Ϊ PORTD_IRQHandler    
                                   //ʹ��PORTD�ж�    
    
    NVIC_SetPriorityGrouping(4);		    //�趨�ж����ȼ�����
	set_irq_priority(UART4_RX_TX_IRQn,3);   //�����ж����ȼ�
	set_irq_priority(PORTA_IRQn,1);   	    //�������ж����ȼ�
    set_irq_priority(PIT0_IRQn ,2);		    //��ʱ���ж����ȼ�
    set_irq_priority(PIT1_IRQn ,0);		    //��ʱ���ж����ȼ�
  	set_irq_priority(PORTD_IRQn ,1);        //����ͣ���ж����ȼ�
    
    enable_irq(PIT0_IRQn);
    enable_irq(PIT1_IRQn);
    enable_irq (PORTD_IRQn); 
    beep_on = 1;
}

/*!
 *  @brief      main����
 *  @since      v1.0
 *  @note       
 */
void main()
{
    System_Init();
    while(1)
    {
        //lptmr_time_start_us();                  //��ʼ��ʱ
        vcan_sendware(UP_Value, sizeof(UP_Value));
        //printf("\n\n����ʱ��Ϊ��%dus",lptmr_time_get_us());
        Change_Level();
        //DELAY_MS(10);
        Beep(50);
        //Superware_Get_Distance();
    }
}
/*!
 *  @brief      PIT0�жϷ�����
 *  @since      v1.0
 *  @note       �����������2ms/��
 */
void PIT0_IRQHandler(void)
{
    TimeCount1++;
    if(TimeCount1 %5 ==0)
    {
        //lptmr_time_start_us();
        Read_ADC();
        //UP_Value[0] = lptmr_time_get_us();//125us
    }
    if(TimeCount1 %10 ==0)
    {
        Server_PID_Ctrl();
    }
    if(TimeCount1>=250)
    {
        TimeCount1 = 0;
        led_turn(LED0);
    }
    PIT_Flag_Clear(PIT0); 	     //�����־λ
}
/*!
 *  @brief      PIT1�жϷ�����
 *  @since      v1.0
 *  @note       ��������жϣ��ٶȿ�������5ms/��
 */
void PIT1_IRQHandler(void)
{
    TimeCount2++;
    Get_Speed();
    Speed_PID_Ctrl();
    //Superware_Get_Distance();
    if(TimeCount2>=200)
    {
        TimeCount2 = 0;
        led_turn(LED1);
    }
    PIT_Flag_Clear(PIT1); 	     //�����־λ
}
/*!
 *  @brief      PORTD�˿��жϷ�����
 *  @since      v1.0
 *  @note       
 */
void PORTD_IRQHandler(void)
{
    if(PORTD_ISFR & (1 << 4))           //PTD4 �����ж�
    {
        PORTD_ISFR  = (1 << 4);        //д1���жϱ�־λ
        StopFlag1 = ~StopFlag1;
        led_turn(LED2);
        beep_on = 2;
    }
}