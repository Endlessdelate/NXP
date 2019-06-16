#include "Stop.h"
/*Magic Don't touch~*/
#define AD_STOP ADC0_SE15      // PTC1

void AD_Stop_Init()
{
    adc_init(AD_STOP);
    pit_init_ms(PIT2,20);                                    //初始化PIT0，定时时间为： 2ms  方向控制周期  
    set_vector_handler(PIT2_VECTORn ,PIT2_IRQHandler);       //设置PIT0的中断服务函数为 PIT0_IRQHandler 
    set_irq_priority(PIT2_IRQn ,0);		                     //定时器中断优先级
    enable_irq (PIT2_IRQn);
}
uint16 adc_ave(ADCn_Ch_e adcn_ch, ADC_nbit bit,uint8 N)
{
  uint32 tmp = 0;
  uint8 i;
  for(i=0;i<N;i++)
    tmp += adc_once(adcn_ch,bit);
  tmp = tmp/N;
  return tmp;
}
void PIT2_IRQHandler(void)
{
   
    unsigned int i,j,temp;
    uint8 ad_value[10];
    uint8 ad_sum;
    for(i=0;i<10;i++)  //存了5组
    {
      ad_value[i]=adc_ave(AD_STOP, ADC_8bit,3);//存10组
    }
    for(j=0;j<10;j++)
    {
      if(ad_value[j]>ad_value[j+1])
      {
        temp = ad_value[j];
        ad_value[j]=ad_value[j+1];
        ad_value[j+1]=temp;	
      }
    }
     ad_sum=(ad_value[2]+ad_value[3]+ad_value[4]+ad_value[5]+ad_value[6]+ad_value[7])/6;
#ifdef STOP_FOUNTION_DEBUG
     UP_Value[0] = ad_sum;
#endif
     if(ad_sum>500)StopFlag = 1;
    //led_turn(LED0);             //闪烁 LED0
    PIT_Flag_Clear(PIT2);       //清中断标志位
}