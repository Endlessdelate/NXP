#include "Superware.h"

uint16 distance = 0;

#define TRIG PTC2
#define ECHO PTC4


void Superware_Init()
{
    gpio_init(TRIG,GPO,0);
    gpio_init(ECHO,GPI,0);
    port_init(ECHO, ALT1 | IRQ_EITHER | PULLDOWN );          //��ʼ�� ECHO �ⲿ�жϹܽ�  ң��ͣ��
    set_vector_handler(PORTC_VECTORn ,PORTC_IRQHandler);    //����PORTC���жϷ�����Ϊ PORTC_IRQHandler  
    set_irq_priority(PORTC_IRQn ,0);        //����ͣ���ж����ȼ�
    enable_irq (PORTC_IRQn);
}

void Superware_Get_Distance()
{
    gpio_set (TRIG,1);
    DELAY_US(10);
    gpio_set (TRIG,0);
    //enable_irq (PORTC_IRQn);   
}
void PORTC_IRQHandler(void)
{
    static uint8 flag = 0;
    if(PORTC_ISFR & (1 << 4))           //PTC4 �����ж�
    {
        PORTC_ISFR  = (1 << 4);        //д1���жϱ�־λ
        if(!flag)
        {
            lptmr_time_start_us();
            flag = 1;
        }
        else if(flag)
        {
            distance = lptmr_time_get_us()*170/1000;
            flag = 0;
        }
    }
}