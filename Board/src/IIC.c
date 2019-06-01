#include "IIC.h"

void IIC_Delay(long t)
{
  volatile long i=t,j;
  while(i--)
  {
    j=1;
    while(j--);
  }
}
/** @brief   �ղ�������ֲ��ʱ�������Ҫ�޸����е���ʱ������
 *           ʹ��GPIO���IICʱʹ��
 * @param
 * @return
 */
static void nops(void)      
{
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
   // asm("nop");
   //asm("nop");
}

//-------------------------------------------------------------------------*
//������: IIC_init                                                        
//��  ��: ��ʼ��IIC                                                       
//��  ��: i2cn:�˿��� I2C0,I2C1                                                                                     
//��  ��: ��                                                              
//��  ��: IIC_init(I2C1);              
//-------------------------------------------------------------------------*
void IMU_IIC_Init(I2Cn i2cn)
{       
  if(i2cn == I2C_0)
  {
    gpio_init(IMU_SCL0, GPO, 1); //��ʼ��SCL0
    gpio_init(IMU_SDA0, GPO, 1); //��ʼ��SDA0

    port_init_NoALT(IMU_SCL0,ODO | PULLUP);
    port_init_NoALT(IMU_SDA0,ODO | PULLUP);
  }
  else
  {
    gpio_init(IMU_SCL1, GPO, 1); //��ʼ��SCL1
    gpio_init(IMU_SDA1, GPO, 1); //��ʼ��SDA1

    port_init_NoALT(IMU_SCL1,ODO | PULLUP);
    port_init_NoALT(IMU_SDA1,ODO | PULLUP);
  }
}

/**
 * @brief    ��ʼIIC����
 * @param
 * @return
 */
void IIC_start(I2Cn i2cn)
{   if(i2cn == I2C_0)
  {
    SDA0_Out;
    I2C0_SCL_H;
    I2C0_SDA_H;
    DELAY_US(2);
    I2C0_SDA_L;
    DELAY_US(2);
    I2C0_SCL_L;
  }
  else
  {
    SDA1_Out;
    I2C1_SCL_H;
    I2C1_SDA_H;
    DELAY_US(2);
    I2C1_SDA_L;
    DELAY_US(2);
    I2C1_SCL_L;
  }
}

/**
 * @brief    ֹͣIIC����
 * @param
 * @return
 */
void IIC_stop(I2Cn i2cn)
{
  if(i2cn == I2C_0)
  {
    SDA0_Out;
    I2C0_SCL_L;
    I2C0_SDA_L;
    DELAY_US(2);
    I2C0_SCL_H;
    DELAY_US(2);
    I2C0_SDA_H;
    DELAY_US(2);
  }
  else
  {
     SDA1_Out;
    I2C1_SCL_L;
    I2C1_SDA_L;
    DELAY_US(2);
    I2C1_SCL_H;
    DELAY_US(2);
    I2C1_SDA_H;
    DELAY_US(2);
  }
}
//�ȴ�Ӧ���ź�  1��Ӧ��ʧ��  0��Ӧ��ɹ�
uint8_t IIC_Wait_Ack(I2Cn i2cn)
{
  uint8_t ucErrTime=0;
   if(i2cn == I2C_0)
   {
        SDA0_In;
        DELAY_US(3);
        I2C0_SCL_H;
        DELAY_US(3);
        while(gpio_get(IMU_SDA0))
        {
          ucErrTime++;
          if(ucErrTime>250)
          {
            IIC_stop(I2C_0);
            return 1;
          }
        }
        I2C0_SCL_L;
        return 0;
   }
   else
   {
        SDA1_In;
        DELAY_US(3);
        I2C1_SCL_H;
        DELAY_US(3);
        while(gpio_get(IMU_SDA1))
        {
          ucErrTime++;
          if(ucErrTime>250)
          {
            IIC_stop(I2C_1);
            return 1;
          }
        }
        I2C1_SCL_L;
        return 0;
   }
  
}
//����Ӧ���ź�
void IIC_Ack(I2Cn i2cn)
{
   if(i2cn == I2C_0)
   {
      I2C0_SCL_L;
      SDA0_Out;
      I2C0_SDA_L;
      DELAY_US(2);
      I2C0_SCL_H;
      DELAY_US(2);
      I2C0_SCL_L;
   }
   else
   {
      I2C1_SCL_L;
      SDA1_Out;
      I2C1_SDA_L;
      DELAY_US(2);
      I2C1_SCL_H;
      DELAY_US(2);
      I2C1_SCL_L;
   }
}
//������Ӧ���ź�
void IIC_NAck(I2Cn i2cn)
{
   if(i2cn == I2C_0)
   {
      I2C0_SCL_L;
      SDA0_Out;
      I2C0_SDA_H;
      DELAY_US(2);
      I2C0_SCL_H;
      DELAY_US(2);
      I2C0_SCL_L;
   }
   else
   {
      I2C1_SCL_L;
      SDA1_Out;
      I2C1_SDA_H;
      DELAY_US(2);
      I2C1_SCL_H;
      DELAY_US(2);
      I2C1_SCL_L;
   }
}
/**
 * @brief    IIC���ݷ���
 * @param    Ҫ���͵�����
 * @return
 */
void IIC_send_byte(I2Cn i2cn,uint8 c)
{
    uint8 t;  
    if(i2cn == I2C_0)
    {
      /*************�������IIC����������*********************/
        SDA0_Out;	    
        I2C0_SCL_L;//����ʱ�ӿ�ʼ���ݴ���
        for(t=0;t<8;t++)
        {           
              
                //IIC_SDA=(txd&0x80)>>7;
		if(c&0x80)
                {
		        I2C0_SDA_H;
                }
		else
                {
			I2C0_SDA_L;
                }
		c<<=1; 	  
		//DELAY_US(2);   
		I2C0_SCL_H;
		DELAY_US(3);
		I2C0_SCL_L;	
		DELAY_US(3);
        }
    }
    else
    {
        /******������ר�ã�д�ٶȷǳ��죬������������Ļ��˸**************/
          SDA1_Out;
          I2C1_SCL_L;
          for(t = 0;t < 8;t++)
          {
              if((c << t) & 0x80)
              {
                I2C1_SDA_H; //�жϷ���λ
              }
              else
              {
                I2C1_SDA_L;
              }
              nops();
              I2C1_SCL_H;
              nops();
              I2C1_SCL_L;
          }
          nops();
          I2C1_SDA_H; //������8bit���ͷ�����׼������Ӧ��λ
          SDA1_In;
          nops();
          I2C1_SCL_H;
          nops();     //sda�����ݼ��Ǵ�Ӧ��λ
          I2C1_SCL_L; //�����Ǵ�Ӧ��λ|��Ҫ���ƺ�ʱ��
    }
}

/**
 * @brief    IIC��ȡ����
 * @param
 * @return   ����������
 */
uint8 IIC_read_byte(I2Cn i2cn)
{
    uint8 i;
    uint8 c;
if(i2cn == I2C_0)
{
    c = 0;
    SDA0_In;
    for(i = 0;i < 8;i++)
    {
       
        I2C0_SCL_L; //��ʱ����Ϊ�ͣ�׼����������λ
        DELAY_US(2);
        I2C0_SCL_H; //��ʱ����Ϊ�ߣ�ʹ��������������Ч
        c <<= 1;
        if(SDAIN0)
          c += 1; //������λ�������յ����ݴ�c
         DELAY_US(2);
    }
    I2C0_SCL_L;
    SDA0_Out;
    return c;
}
else
{
      c = 0;
    SDA1_In;
    for(i = 0;i < 8;i++)
    {
       
        I2C1_SCL_L; //��ʱ����Ϊ�ͣ�׼����������λ
        DELAY_US(2);
        I2C1_SCL_H; //��ʱ����Ϊ�ߣ�ʹ��������������Ч
        c <<= 1;
        if(SDAIN1)
          c += 1; //������λ�������յ����ݴ�c
         DELAY_US(2);
    }
    I2C1_SCL_L;
    SDA1_Out;
    return c;
}
}

/**
 * @brief    IICд����
 * @param    SlaveID �ӻ���ַ
 * @param    reg     �Ĵ�����ַ
 * @param    Data    Ҫд�������
 * @return
 */
void  IMU_WriteRegister(I2Cn i2cn,uint8 SlaveID, uint8 reg, uint8 Data)
{
    IIC_start(i2cn);
    IIC_send_byte(i2cn,SlaveID << 1); //д���豸ID��д�ź�
    IIC_send_byte(i2cn,reg);
    IIC_send_byte(i2cn,Data);
    IIC_stop(i2cn);
}

/**
 * @brief    IIC������
 * @param    SlaveID �ӻ���ַ
 * @param    reg     �Ĵ�����ַ
 * @return   ����������
 */
uint8 IMU_ReadRegister(I2Cn i2cn,uint8 SlaveID, uint8 reg)
{
    uint8 ret;

    IIC_start(i2cn);
    IIC_send_byte(i2cn,SlaveID << 1); //д���豸ID��д�ź�
    IIC_send_byte(i2cn,reg);
    IIC_start(i2cn);
    IIC_send_byte(i2cn,(SlaveID << 1) + 1); //д���豸ID�����ź�
    ret = IIC_read_byte(i2cn);
    IIC_stop(i2cn);

    return ret;
}