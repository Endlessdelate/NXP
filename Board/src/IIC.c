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
/** @brief   空操作（移植的时候可能需要修改其中的延时个数）
 *           使用GPIO软件IIC时使用
 * @param
 * @return
 */
static void nops(void)      
{
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
}

//-------------------------------------------------------------------------*
//函数名: IIC_init                                                        
//功  能: 初始化IIC                                                       
//参  数: i2cn:端口名 I2C0,I2C1                                                                                     
//返  回: 无                                                              
//简  例: IIC_init(I2C1);              
//-------------------------------------------------------------------------*
void IMU_IIC_Init(I2Cn i2cn)
{       
  if(i2cn == I2C_0)
  {
    gpio_init(IMU_SCL0, GPO, 1); //初始化SCL0
    gpio_init(IMU_SDA0, GPO, 1); //初始化SDA0

    port_init_NoALT(IMU_SCL0,ODO | PULLUP);
    port_init_NoALT(IMU_SDA0,ODO | PULLUP);
  }
  else
  {
    gpio_init(IMU_SCL1, GPO, 1); //初始化SCL1
    gpio_init(IMU_SDA1, GPO, 1); //初始化SDA1

    port_init_NoALT(IMU_SCL1,ODO | PULLUP);
    port_init_NoALT(IMU_SDA1,ODO | PULLUP);
  }
}

/**
 * @brief    开始IIC传输
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
 * @brief    停止IIC传输
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
//等待应答信号  1：应答失败  0：应答成功
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
//产生应答信号
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
//不产生应答信号
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
 * @brief    IIC数据发送
 * @param    要发送的数据
 * @return
 */
void IIC_send_byte(I2Cn i2cn,uint8 c)
{
    uint8 t;  
    if(i2cn == I2C_0)
    {
      /*************常规低速IIC，陀螺仪用*********************/
        SDA0_Out;	    
        I2C0_SCL_L;//拉低时钟开始数据传输
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
        /******按键板专用，写速度非常快，几乎看不到屏幕闪烁**************/
          SDA1_Out;
          I2C1_SCL_L;
          for(t = 0;t < 8;t++)
          {
              if((c << t) & 0x80)
              {
                I2C1_SDA_H; //判断发送位
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
          I2C1_SDA_H; //发送完8bit，释放总线准备接收应答位
          SDA1_In;
          nops();
          I2C1_SCL_H;
          nops();     //sda上数据即是从应答位
          I2C1_SCL_L; //不考虑从应答位|但要控制好时序
    }
}

/**
 * @brief    IIC读取数据
 * @param
 * @return   读到的数据
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
       
        I2C0_SCL_L; //置时钟线为低，准备接收数据位
        DELAY_US(2);
        I2C0_SCL_H; //置时钟线为高，使数据线上数据有效
        c <<= 1;
        if(SDAIN0)
          c += 1; //读数据位，将接收的数据存c
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
       
        I2C1_SCL_L; //置时钟线为低，准备接收数据位
        DELAY_US(2);
        I2C1_SCL_H; //置时钟线为高，使数据线上数据有效
        c <<= 1;
        if(SDAIN1)
          c += 1; //读数据位，将接收的数据存c
         DELAY_US(2);
    }
    I2C1_SCL_L;
    SDA1_Out;
    IIC_Ack(I2C_1);
    return c;
}
}

/**
 * @brief    IIC写操作
 * @param    SlaveID 从机地址
 * @param    reg     寄存器地址
 * @param    Data    要写入的数据
 * @return
 */
void  IMU_WriteRegister(I2Cn i2cn,uint8 SlaveID, uint8 reg, uint8 Data)
{
    IIC_start(i2cn);
    IIC_send_byte(i2cn,SlaveID << 1); //写入设备ID及写信号
    IIC_send_byte(i2cn,reg);
    IIC_send_byte(i2cn,Data);
    IIC_stop(i2cn);
}

/**
 * @brief    IIC读操作
 * @param    SlaveID 从机地址
 * @param    reg     寄存器地址
 * @return   读到的数据
 */
uint8 IMU_ReadRegister(I2Cn i2cn,uint8 SlaveID, uint8 reg)
{
    uint8 ret;

    IIC_start(i2cn);
    IIC_send_byte(i2cn,SlaveID << 1); //写入设备ID及写信号
    IIC_send_byte(i2cn,reg);
    IIC_start(i2cn);
    IIC_send_byte(i2cn,(SlaveID << 1) + 1); //写入设备ID及读信号
    ret = IIC_read_byte(i2cn);
    IIC_stop(i2cn);

    return ret;
}