/*！
 * @file       main.c
 * @brief      猎户座主程序
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
void PIT1_IRQHandler(void);    //脉冲计数中断
void PORTD_IRQHandler(void);   //PORTC端口中断服务函数
void key_Stop(void);           //按键停车


/*!
 *  @brief      系统初始化函数
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
    OLED_Init();          //OLED初始化
    Key_Init();
    Menu_Init();          //菜单初始化
    PID_Init();
    Encoder_init();
    Motor_Init();
    Server_Init();
    Beep_Init();
    Superware_Init();
    AD_Stop_Init();
    
    pit_init_ms(PIT0,2);                                     //初始化PIT0，定时时间为： 2ms  方向控制周期  
    set_vector_handler(PIT0_VECTORn ,PIT0_IRQHandler);       //设置PIT0的中断服务函数为 PIT0_IRQHandler    
                                       //使能PIT0中断  
        
    pit_init_ms(PIT1,5);                                    //初始化PIT1，定时时间为： 5ms   速度控制周期
    set_vector_handler(PIT1_VECTORn ,PIT1_IRQHandler);      //设置PIT1的中断服务函数为 PIT1_IRQHandler    
                                      //使能PIT1中断  
        
    port_init(PTD4, ALT1 | IRQ_RISING | PULLDOWN );          //初始化 PTD4 外部中断管脚  遥控停车
    set_vector_handler(PORTD_VECTORn ,PORTD_IRQHandler);    //设置PORTD的中断服务函数为 PORTD_IRQHandler    
                                   //使能PORTD中断    
    
    NVIC_SetPriorityGrouping(4);		    //设定中断优先级分组
	set_irq_priority(UART4_RX_TX_IRQn,3);   //串口中断优先级
	set_irq_priority(PORTA_IRQn,1);   	    //按键板中断优先级
    set_irq_priority(PIT0_IRQn ,2);		    //定时器中断优先级
    set_irq_priority(PIT1_IRQn ,0);		    //定时器中断优先级
  	set_irq_priority(PORTD_IRQn ,1);        //按键停车中断优先级
    
    enable_irq(PIT0_IRQn);
    enable_irq(PIT1_IRQn);
    enable_irq (PORTD_IRQn); 
    beep_on = 1;
}

/*!
 *  @brief      main函数
 *  @since      v1.0
 *  @note       
 */
void main()
{
    System_Init();
    while(1)
    {
        //lptmr_time_start_us();                  //开始计时
        vcan_sendware(UP_Value, sizeof(UP_Value));
        //printf("\n\n串口时间为：%dus",lptmr_time_get_us());
        Change_Level();
        //DELAY_MS(10);
        Beep(50);
        //Superware_Get_Distance();
    }
}
/*!
 *  @brief      PIT0中断服务函数
 *  @since      v1.0
 *  @note       方向控制周期2ms/次
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
    PIT_Flag_Clear(PIT0); 	     //清除标志位
}
/*!
 *  @brief      PIT1中断服务函数
 *  @since      v1.0
 *  @note       脉冲计数中断，速度控制周期5ms/次
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
    PIT_Flag_Clear(PIT1); 	     //清除标志位
}
/*!
 *  @brief      PORTD端口中断服务函数
 *  @since      v1.0
 *  @note       
 */
void PORTD_IRQHandler(void)
{
    if(PORTD_ISFR & (1 << 4))           //PTD4 触发中断
    {
        PORTD_ISFR  = (1 << 4);        //写1清中断标志位
        StopFlag1 = ~StopFlag1;
        led_turn(LED2);
        beep_on = 2;
    }
}